use crate::cartridge::{Cartridge, Controller, Ram as CartridgeRam, Rom};
use crate::error::Result;

/// Generic traits that provide access to some memory
/// 
/// `A` is the address size, `V` is the value
pub trait MemoryRead<A, V> {
    /// Read a single value from an address
    fn read(&self, addr: A) -> V;
}

pub trait MemoryWrite<A, V> {
    /// Write a single value to an address
    fn write(&mut self, addr: A, value: V);
}

/// Internal console work RAM
///
/// * 0xC000 - 0xCFFF: Bank 0,   4K, static
/// * 0xD000 - 0xDFFF: Bank 1,   4K  (DMG mode)
#[cfg_attr(feature="save", derive(serde::Serialize), derive(serde::Deserialize))]
pub struct Ram {
    data: Vec<u8>,
}

impl Ram {
    const BANK_SIZE: usize = 4096; // 4K
    pub const BASE_ADDR: u16 = 0xC000;
    pub const LAST_ADDR: u16 = 0xDFFF;

    pub fn new() -> Self {
        Self {
            data: vec![0xFFu8; Self::BANK_SIZE * 2 as usize],
        }
    }
}

impl MemoryRead<u16, u8> for Ram {
    #[inline]
    fn read(&self, addr: u16) -> u8 {
        let addr = (addr - Self::BASE_ADDR) as usize;

        self.data[addr]
    }
}

impl MemoryWrite<u16, u8> for Ram {
    #[inline]
    fn write(&mut self, addr: u16, value: u8) {
        let addr = (addr - Self::BASE_ADDR) as usize;

        self.data[addr] = value;
    }
}

// TODO IO devices

pub enum MemoryType {
    Rom,
    Ram,
    CartridgeRam,
    Vram,
    Hram,
    Other,
}

impl std::fmt::Display for MemoryType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            MemoryType::Rom => write!(f, "ROM"),
            MemoryType::Ram => write!(f, "RAM"),
            MemoryType::CartridgeRam => write!(f, "CRAM"),
            MemoryType::Vram => write!(f, "VRAM"),
            MemoryType::Hram => write!(f, "HRAM"),
            MemoryType::Other => write!(f, "OTHER"),
        }
    }
}

/// 64K memory map
#[cfg_attr(feature = "save", derive(serde::Serialize), derive(serde::Deserialize))]
pub struct MemoryBus {
    /// ROM: 0x0000 - 0x7FFF
    /// Cart RAM: 0xA000 - 0xBFFF
    controller: Controller,
    /// VRAM: 0x8000 - 0x9FFF

    /// Work RAM: 0xC000 - 0xDFFF
    ram: Ram,

    /// ignored

    /// IO: 0xFF00-0xFF7F
     
    /// High RAM: 0xFF80 - 0xFFFE
    high_ram: Box<[u8]>,

    /// Interrupt enable  - 0xFFFF
    pub int_enable: u8,
}

impl MemoryBus {
    pub const HRAM_BASE_ADDR: u16 = 0xFF80;
    pub const HRAM_LAST_ADDR: u16 = 0xFFFE;

    pub fn new() -> Self {
        Self {
            controller: Controller::new(),
            ram: Ram::new(),
            high_ram: Box::new([0xFFu8; 0x80]),
            int_enable: 0,
        }
    }

    pub fn from_cartridge(cartridge: Cartridge) -> Result<Self> {
        let controller = Controller::from_cartridge(cartridge)?;

        Ok(Self {
            controller,
            ram: Ram::new(),
            high_ram: Box::new([0xFFu8, 0x80]),
            int_enable: 0,
        })
    }

    /// Reset the memory bus
    pub fn reset(&mut self) {
        self.controller.reset();

        self.ram = Ram::new();
        self.high_ram = Box::new([0xFFu8; 0x80]);
        self.int_enable = 0;
    }

    /// Given an address, return the type of memory and bank number
    pub fn memory_info(&self, addr: u16) -> (MemoryType, u16) {
        if addr >= Rom::BASE_ADDR && addr < Rom::BASE_ADDR + Rom::BANK_SIZE as u16 {
            (MemoryType::Rom, self.controller.rom.active_bank_0)
        } else if addr >= Rom::BASE_ADDR + Rom::BANK_SIZE as u16 && addr <=Rom::LAST_ADDR {
            (MemoryType::Rom, self.controller.rom.active_bank_1)
        } else if addr >= CartridgeRam::BASE_ADDR && addr <=CartridgeRam::LAST_ADDR {
            (MemoryType::CartridgeRam, match &self.controller.ram {
                None => 0,
                Some(ram) => ram.active_bank as u16,
            })
        } else if addr >= Ram::BASE_ADDR && addr <= Ram::LAST_ADDR {
            (MemoryType::Ram, 0)
        } else if addr >= Self::HRAM_BASE_ADDR && addr <= Self::HRAM_LAST_ADDR {
            (MemoryType::Hram, 0)
        } else {
            (MemoryType::Other, 0)
        }
    }

    pub fn controller(&self) -> &Controller {
        &self.controller
    }

    pub fn controller_mut(&mut self) -> &mut Controller {
        &mut self.controller
    }
}

impl MemoryRead<u16, u8> for MemoryBus {
    /// Read a single byte from an arbitrary memory address
    fn read(&self, addr: u16) -> u8 {
        match addr {
            Rom::BASE_ADDR..=Rom::LAST_ADDR | CartridgeRam::BASE_ADDR..=CartridgeRam::LAST_ADDR => self.controller.read(addr),
            Ram::BASE_ADDR..=Ram::LAST_ADDR => self.ram.read(addr),
            0xE000..=0xFDFF => {
                // Echo RAM
                self.read(addr - 0x2000)
            }
            0xFEA0..=0xFEFF => {
                // Prohibited area
                // From Pan Docs:
                // "Returns the high nibble of the lower address byte twice,
                //    e.g. FFAx returns AA, FFBx returns BB, and so forth."
                let upper_nibble = ((addr & 0xF0) >> 4) as u8;
                upper_nibble << 4 | upper_nibble
            }
            0xFF80..=0xFFFE => {
                let addr = addr as usize - 0xFF80;
                self.high_ram[addr]
            }
            0xFFFF => self.int_enable,
            _ => unimplemented!(),
        }
    }
}

impl MemoryWrite<u16, u8> for MemoryBus {
    fn write(&mut self, addr: u16, value: u8) {
        match addr {
            Rom::BASE_ADDR..=Rom::LAST_ADDR | CartridgeRam::BASE_ADDR..=CartridgeRam::LAST_ADDR => self.controller.write(addr, value),
            Ram::BASE_ADDR..=Ram::LAST_ADDR => self.ram.write(addr, value),
            0xFF80..=0xFFFE => {
                let addr = addr as usize - 0xFF80;
                self.high_ram[addr] = value;
            }
            0xFFFF => self.int_enable = value,
            _ => unimplemented!(),
        }
    }
}

/// Write a 16-bit word to memory
impl MemoryWrite<u16, u16> for MemoryBus {
    #[inline]
    fn write(&mut self, addr: u16, value: u16) {
        let value = value.to_le_bytes(); // Game Boy is little-endian
        self.write(addr, value[0]);
        self.write(addr + 1, value[1]);
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn ram_operations() {
        let mut ram = Ram::new();

        ram.write(Ram::BASE_ADDR, 0x66u8);
        let value: u8 = ram.read(Ram::BASE_ADDR);
        assert_eq!(value, 0x66);

        ram.write(Ram::BASE_ADDR + 0x1234u16, 0x66u8);
        let value: u8 = ram.read(Ram::BASE_ADDR + 0x1234u16);
        assert_eq!(value, 0x66);
    }
}