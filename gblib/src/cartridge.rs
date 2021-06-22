use std::convert::TryFrom;

use crate::error::{CartridgeError, Error, Result};
use crate::memory::{MemoryRead, MemoryWrite};

// Cartridge RAM size
#[derive(Copy, Clone, Debug, PartialEq)]
#[cfg_attr(feature="save", derive(serde::Serialize), derive(serde::Deserialize))]
#[repr(u8)]
pub enum RamSize {
    // In this order to make translating from cartridge header easier
    NotPresent,
    _2K,
    _8K,
    _32K,
    _128K,
    _64K,
}

/// Convert from RAM size variant to RAM size in bytes
impl From<RamSize> for usize {
    fn from(s: RamSize) -> usize {
        match s {
            RamSize::NotPresent => 0,
            RamSize::_2K => 2 * 1024,
            RamSize::_8K => 8 * 1024,
            RamSize::_32K => 32 * 1024,
            RamSize::_64K => 64 * 1024,
            RamSize::_128K => 128 * 1024,
        }
    }
}

/// Convert from raw RAM size in bytes to a RAM size variant
impl TryFrom<usize> for RamSize {
    type Error = Error;

    fn try_from(size: usize) -> std::result::Result<Self, Self::Error> {
        match size {
            x if x == 2 * 1024 => Ok(RamSize::_2K),
            x if x == 8 * 1024 => Ok(RamSize::_8K),
            x if x == 32 * 1024 => Ok(RamSize::_32K),
            x if x == 64 * 1024 => Ok(RamSize::_64K),
            x if x == 128 * 1024 => Ok(RamSize::_128K),
            _ => Err(Error::InvalidValue(format!("Invalid RAM size: {}", size))),
        }
    }
}

impl TryFrom<u8> for RamSize {
    type Error = Error;

    fn try_from(size: u8) -> std::result::Result<Self, Self::Error> {
        match size {
            x if x == RamSize::_2K as u8 => Ok(RamSize::_2K),
            x if x == RamSize::_8K as u8 => Ok(RamSize::_8K),
            x if x == RamSize::_32K as u8 => Ok(RamSize::_32K),
            x if x == RamSize::_64K as u8 => Ok(RamSize::_64K),
            x if x == RamSize::_128K as u8 => Ok(RamSize::_128K),
            x if x == RamSize::NotPresent as u8 => Ok(RamSize::NotPresent),
            _ => Err(Error::InvalidValue(format!("Invalid RAM size: {}", size))),
        }
    }
}

/// Cartridge RAM
#[cfg_attr(feature="save", derive(serde::Serialize), derive(serde::Deserialize))]
pub struct Ram {
    data: Vec<u8>,
    pub(crate) active_bank: u8,
    num_banks: u8,
    ram_size: RamSize,

    /// Dirty flag set on write to RAM
    pub(crate) is_dirty: bool,
}

impl Ram {
    const BANK_SIZE: usize = 8 * 1024; // 8K
    pub const BASE_ADDR: u16 = 0xA000;
    pub const LAST_ADDR: u16 = 0xBFFF;

    /// Create empty RAM
    fn new(ram_size: RamSize) -> Option<Self> {
        match ram_size {
            RamSize::NotPresent => {
                None
            }
            _ => {
                // Get raw RAM size in bytes
                let size = usize::from(ram_size);
                let data = vec![0xFFu8; size];
                let num_banks = if ram_size == RamSize::_2K {
                    1
                } else {
                    (size / Self::BANK_SIZE) as u8
                };

                Some(Self {
                    data,
                    active_bank: 0,
                    num_banks,
                    ram_size,
                    is_dirty: false,
                })
            }
        }
    }

    /// Create RAM from raw bytes
    /// 
    /// This function will return an error if the data does not have
    /// a valid RAM size
    pub fn from_bytes(data: &[u8]) -> Result<Self> {
        let size = data.len();
        let ram_size = RamSize::try_from(size)?;

        let data = data.to_owned();
        let num_banks = if ram_size == RamSize::_2K {
            1
        } else {
            (size / Self::BANK_SIZE) as u8
        };

        Ok(Self {
            data,
            active_bank: 0,
            num_banks,
            ram_size,
            is_dirty: false
        })
    }

    pub(crate) fn data(&self) -> &[u8] {
        &self.data
    }

    /// Handle a bank change request
    pub(crate) fn set_bank(&mut self, bank: u8) {
        if self.num_banks == 1 {
            log::warn!("Switching bank on unbanked RAM!");
        }

        self.active_bank = bank & (self.num_banks - 1);
    }
}

impl MemoryRead<u16, u8> for Ram {
    /// Read a byte of data from the current active bank
    #[inline]
    fn read(&self, addr: u16) -> u8 {
        let addr = (addr - Self::BASE_ADDR) as usize;
        let bank_offset = self.active_bank as usize * Self::BANK_SIZE;
        self.data[bank_offset + addr]
    }
}

impl MemoryWrite<u16, u8> for Ram {
    /// Write a byte of data to the current active bank
    #[inline]
    fn write(&mut self, addr: u16, value: u8) {
        let addr = (addr - Self::BASE_ADDR) as usize;
        let bank_offset = self.active_bank as usize * Self::BANK_SIZE;
        self.data[bank_offset + addr] = value;
        self.is_dirty = true;
    }
}

/// ROM size
#[derive(Copy, Clone, Debug, PartialEq)]
#[cfg_attr(feature="save", derive(serde::Serialize), derive(serde::Deserialize))]
#[repr(u8)]
pub enum RomSize {
    _32K,
    _64K,
    _128K,
    _256K,
    _512K,
    _1M,
    _2M,
    _4M,
    _8M,
    _1_1M = 0x52,
    _1_2M,
    _1_5M,
}

/// Convert from ROM size variant to raw size in bytes
impl From<RomSize> for usize {
    fn from(s: RomSize) -> usize {
        match s {
            RomSize::_32K => 2 * Rom::BANK_SIZE, 
            RomSize::_64K => 4 * Rom::BANK_SIZE, 
            RomSize::_128K => 8 * Rom::BANK_SIZE, 
            RomSize::_256K => 16 * Rom::BANK_SIZE, 
            RomSize::_512K => 32 * Rom::BANK_SIZE, 
            RomSize::_1M => 64 * Rom::BANK_SIZE, 
            RomSize::_1_1M => 72 * Rom::BANK_SIZE, 
            RomSize::_1_2M => 80 * Rom::BANK_SIZE, 
            RomSize::_1_5M => 96 * Rom::BANK_SIZE, 
            RomSize::_2M => 128 * Rom::BANK_SIZE, 
            RomSize::_4M => 256 * Rom::BANK_SIZE,
            RomSize::_8M => 512 * Rom::BANK_SIZE, 
        }
    }
}

impl TryFrom<u8> for RomSize {
    type Error = Error;

    fn try_from(val: u8) -> std::result::Result<Self, Self::Error> {
        match val {
            x if x == RomSize::_32K as u8 => Ok(RomSize::_32K),
            x if x == RomSize::_64K as u8 => Ok(RomSize::_64K),
            x if x == RomSize::_128K as u8 => Ok(RomSize::_128K),
            x if x == RomSize::_256K as u8 => Ok(RomSize::_256K),
            x if x == RomSize::_512K as u8 => Ok(RomSize::_512K),
            x if x == RomSize::_1M as u8 => Ok(RomSize::_1M),
            x if x == RomSize::_1_1M as u8 => Ok(RomSize::_1_1M),
            x if x == RomSize::_1_2M as u8 => Ok(RomSize::_1_2M),
            x if x == RomSize::_1_5M as u8 => Ok(RomSize::_1_5M),
            x if x == RomSize::_2M as u8 => Ok(RomSize::_2M),
            x if x == RomSize::_4M as u8 => Ok(RomSize::_4M),
            x if x == RomSize::_8M as u8 => Ok(RomSize::_8M),
            _ => Err(Error::InvalidValue(format!("Invalid ROM size: {}", val))),
        }
    }
}

#[cfg_attr(feature="save", derive(serde::Serialize), derive(serde::Deserialize))]
/// ROM
pub struct Rom {
    /// ROM data for all banks
    #[cfg_attr(feature="save", serde(skip))]
    data: Vec<u8>,

    /// Active bank 0
    pub(crate) active_bank_0: u16,

    /// Actibe bank 1
    pub(crate) active_bank_1: u16,

    /// Total number of banks
    num_banks: u16,

    /// Size of ROM
    rom_size: RomSize,
}

impl Rom {
    pub const BANK_SIZE: usize = 16 * 1024; // 16 K
    pub const BASE_ADDR: u16 = 0x0000;
    pub const LAST_ADDR: u16 = 0x7FFF;

    pub fn new(rom_size: RomSize) -> Self {
        let size = usize::from(rom_size);
        let num_banks = size / Self::BANK_SIZE;

        Self {
            data: vec![0xFF; size],
            active_bank_0: 0,
            active_bank_1: 1,
            num_banks: num_banks as u16,
            rom_size,
        }
    }

    /// Construct a ROM from raw butes
    pub fn from_bytes(data: &[u8], rom_size: RomSize) -> Self {
        let size = usize::from(rom_size);
        let num_banks = size / Self::BANK_SIZE;
        let data = data.to_owned();

        assert!(size == data.len());

        Self {
            data,
            active_bank_0: 0,
            active_bank_1: 1,
            num_banks: num_banks as u16,
            rom_size,
        }
    }

    /// Load data into this ROM
    pub fn load(&mut self, data: Vec<u8>) {
        let size = usize::from(self.rom_size);
        assert!(size == data.len());
        self.data = data;
    }

    pub fn update_bank_0(&mut self, bank: u16) {
        assert!(bank < self.num_banks);
        self.active_bank_0 = bank;
    }

    pub fn update_bank_1(&mut self, bank: u16) {
        assert!(bank < self.num_banks);
        self.active_bank_1 = bank;
    }
}

impl MemoryRead<u16, u8> for Rom {
    #[inline]
    fn read(&self, addr: u16) -> u8 {
        let addr = addr as usize;

        match addr {
            0x0000..=0x3FFF => {
                // Bank 0
                let bank_offset = self.active_bank_0 as usize * Self::BANK_SIZE;
                self.data[bank_offset + addr]
            }
            0x4000..=0x7FFF => {
                // Bank 1
                let addr = addr - 0x4000;
                let bank_offset = self.active_bank_1 as usize * Self::BANK_SIZE;
                self.data[bank_offset + addr]
            }
            _ => unreachable!("Unexpected read from {}", addr),
        }
    }
}

#[cfg(test)]
impl MemoryWrite<u16, u8> for Rom {
    #[inline]
    fn write(&mut self, addr: u16, value: u8) {
        let addr = addr as usize;

        match addr {
            0x0000..=0x3FFF => {
                // Bank 0
                let bank_offset = self.active_bank_0 as usize * Self::BANK_SIZE;
                self.data[bank_offset + addr] = value;
            }
            0x4000..=0x7FFF => {
                let addr = addr - 0x4000;
                let bank_offset = self.active_bank_0 as usize * Self::BANK_SIZE;
                self.data[bank_offset + addr] = value;
            }
            _ => unreachable!("Unexpected write to {}", addr),
        }
    }
}

#[cfg_attr(feature = "save", derive(serde::Serialize), derive(serde::Deserialize))]
/// Cartridge ROM controller
pub struct Controller {
    /// ROM
    pub(crate) rom: Rom,
    
    /// RAM
    pub(crate) ram: Option<Ram>,

    /// ROM ize
    rom_size: RomSize,

    /// RAM size
    ram_size: RamSize,

    /// Cartridge type
    cartridge_type: CartridgeType,

    /// Bank mode
    banking_mode: bool,

    /// RAM enable flag
    ram_enable: bool,

    /// RAM/ROM bank select register
    ram_rom_bank: u8,
}

impl Controller {
    /// Create a new default controller
    pub fn new() -> Self {
        let rom_size = RomSize::_32K;
        let ram_size = RamSize::_8K;

        Self {
            rom: Rom::new(rom_size),
            ram: Ram::new(ram_size),
            rom_size,
            ram_size,
            cartridge_type: CartridgeType::MBC1,
            banking_mode: false,
            ram_enable: false,
            ram_rom_bank: 0,
        }
    }

    /// Create a controller from a cartridge
    pub fn from_cartridge(cartridge: Cartridge) -> Result<Self> {
        /// Extract ROM and RAM from cartridge header
        let cartridge_type = cartridge.cartridge_type()?;
        let rom_size = cartridge.rom_size()?;
        let ram_size = cartridge.ram_size()?;
        let rom = Rom::from_bytes(&cartridge.data, rom_size);
        let ram = Ram::new(ram_size);
        
        Ok(Self {
            rom,
            ram,
            rom_size,
            ram_size,
            cartridge_type,
            banking_mode: false,
            ram_enable: false,
            ram_rom_bank: 0,
        })
    }

    /// Load raw RAM data into this controller
    pub fn load_ram(&mut self, data: &[u8]) -> Result<()> {
        let ram = Ram::from_bytes(data)?;
        self.ram.replace(ram);
        Ok(())
    }

    /// Load data into this controller from ROM data
    #[cfg(feature = "save")]
    pub(crate) fn load_rom(&mut self, data: Vec<u8>) {
        self.rom.load(data);
    }

    /// Reset this controller
    pub(crate) fn reset(&mut self) {
        self.ram = Ram::new(self.ram_size);
    }
}

impl MemoryRead<u16, u8> for Controller {
    fn read(&self, addr: u16) -> u8 {
        match addr {
            Rom::BASE_ADDR..=Rom::LAST_ADDR => self.rom.read(addr),
            Ram::BASE_ADDR..=Ram::LAST_ADDR => self.ram.as_ref().unwrap().read(addr),
            _ => unreachable!("Invalid read from 0x{:X}", addr),
        }
    }
}

// TODO support more MBC controllers
impl MemoryWrite<u16, u8> for Controller {
    #[inline]
    fn write(&mut self, addr: u16, value: u8) {
        match addr {
            0x0000..=0x1FFF if self.cartridge_type.is_mbc1() => {
                // Cartridge RAM enable/disable
                if value & 0xF == 0xA {
                    self.ram_enable = true
                } else {
                    self.ram_enable = false;
                }
            }
            0x2000..=0x3FFF if self.cartridge_type.is_mbc1() => {
                // MBC1 ROM bank select
                let value = value & 0x1F;
                let value = if value == 0 { 1 } else { value };
                self.rom.update_bank_1(value as u16);
            }
            0x4000..=0x5FFF if self.cartridge_type.is_mbc1() => {
                // MBC1 RAM bank select or upper 2 bits of ROM bank
                let value = value & 0x3;

                if usize::from(self.ram_size) == RamSize::_32K.into() {
                    // Switch RAM bank
                    self.ram.as_mut().unwrap().set_bank(value);
                } else if usize::from(self.rom_size) >= RomSize::_1M.into() {
                    // For large ROM carts there are 2 options
                    if !self.banking_mode {
                        // Simple banking mode
                        let value = self.rom.active_bank_1 | (value as u16) << 5;
                        self.rom.update_bank_1(value);
                    } else {
                        // Advanced banking mode
                        let bank0 = match value {
                            0 => 0,
                            1 => 0x20,
                            2 => 0x40,
                            3 => 0x60,
                            _ => unreachable!(),
                        };

                        self.rom.update_bank_0(bank0);
                    }
                }

                self.ram_rom_bank = value;
            }
            0x6000..=0x7FFF if self.cartridge_type.is_mbc1() => {
                // MBC1 banking mode select
                let large_ram = usize::from(self.ram_size) >= RamSize::_32K.into();
                let large_rom = usize::from(self.rom_size) >= RomSize::_1M.into();
                if !large_ram && !large_rom {
                    return; // No effect on small carts
                }

                let banking_mode = value & 0x01 == 1;

                if self.ram_enable && large_ram && banking_mode {
                    // Large RAM, switch to previously selected bank immediately
                    self.ram.as_mut().unwrap().set_bank(self.ram_rom_bank);
                }

                self.banking_mode = banking_mode;
            }

            Ram::BASE_ADDR..=Ram::LAST_ADDR => {
                if self.ram_enable {
                    self.ram.as_mut().unwrap().write(addr, value)
                }
            }

            // Ignore all other writes
            _ => ()
        }
    }
}

/// GB cartridge types
#[derive(Copy, Clone, Debug, PartialEq)]
#[cfg_attr(feature = "save", derive(serde::Serialize), derive(serde::Deserialize))]
#[repr(u8)]
pub enum CartridgeType {
    ROM,
    MBC1,
    MBC1_RAM,
    MBC1_RAM_BATT,
    MBC2 = 0x5,
    MBC2_BATT,
    RAM = 0x8,
    RAM_BATT,
    MMM01 = 0xB,
    MMM01_RAM,
    MMM01_RAM_BATT,
    MBC3_TIMER_BATT,
    MBC3_TIMER_RAM_BATT,
    MBC3,
    MBC3_RAM,
    MBC3_RAM_BATT,
    MBC5 = 0x19,
    MBC5_RAM,
    MBC5_RAM_BATT,
    MBC5_RUMBLE,
    MBC5_RUMBLE_RAM,
    MBC5_RUMBLE_RAM_BATT,
    POCKET_CAMERA = 0xFC,
    BANDAI_TAMA5,
    HuC3,
    HuC1_RAM_BATT,
}

impl CartridgeType {
    pub fn is_none(&self) -> bool {
        use CartridgeType::*;
        match self {
            ROM | RAM | RAM_BATT => true,
            _ => false,
        }
    }

    pub fn is_mbc1(&self) -> bool {
        use CartridgeType::*;
        match self {
            MBC1 | MBC1_RAM | MBC1_RAM_BATT => true,
            _ => false,
        }
    }

    pub fn is_mbc2(&self) -> bool {
        use CartridgeType::*;
        match self {
            MBC2 | MBC2_BATT => true,
            _ => false,
        }
    }

    pub fn is_mbc3(&self) -> bool {
        use CartridgeType::*;
        match self {
            MBC3 | MBC3_RAM | MBC3_RAM_BATT | MBC3_TIMER_BATT | MBC3_TIMER_RAM_BATT => true,
            _ => false,
        }
    }

    pub fn is_mbc5(&self) -> bool {
        use CartridgeType::*;
        match self {
            MBC5 | MBC5_RAM | MBC5_RAM_BATT | MBC5_RUMBLE | MBC5_RUMBLE_RAM | MBC5_RUMBLE_RAM_BATT => true,
            _ => false,
        }
    }

    pub fn is_battery_backed(&self) -> bool {
        use CartridgeType::*;
        match self {
            RAM_BATT | MBC1_RAM_BATT | MBC3_RAM_BATT | MBC3_TIMER_RAM_BATT | MBC5_RAM_BATT | MBC5_RUMBLE_RAM_BATT => true,
            _ => false,
        }
    }

    pub fn is_rtc(&self) -> bool {
        use CartridgeType::*;
        match self {
            MBC3_TIMER_BATT | MBC3_TIMER_RAM_BATT => true,
            _ => false,
        }
    }
}

impl TryFrom<u8> for CartridgeType {
    type Error = Error;

    fn try_from(val: u8) -> std::result::Result<Self, Self::Error> {
        match val {
            x if x == CartridgeType::ROM as u8 => Ok(CartridgeType::ROM),
            x if x == CartridgeType::MBC1 as u8 => Ok(CartridgeType::MBC1),
            x if x == CartridgeType::MBC1_RAM as u8 => Ok(CartridgeType::MBC1_RAM),
            x if x == CartridgeType::MBC1_RAM_BATT as u8 => Ok(CartridgeType::MBC1_RAM_BATT),
            x if x == CartridgeType::MBC2 as u8 => Ok(CartridgeType::MBC2),
            x if x == CartridgeType::MBC2_BATT as u8 => Ok(CartridgeType::MBC2_BATT),
            x if x == CartridgeType::RAM as u8 => Ok(CartridgeType::RAM),
            x if x == CartridgeType::RAM_BATT as u8 => Ok(CartridgeType::RAM_BATT),
            x if x == CartridgeType::MMM01 as u8 => Ok(CartridgeType::MMM01),
            x if x == CartridgeType::MMM01_RAM as u8 => Ok(CartridgeType::MMM01_RAM),
            x if x == CartridgeType::MMM01_RAM_BATT as u8 => Ok(CartridgeType::MMM01_RAM_BATT),
            x if x == CartridgeType::MBC3_TIMER_BATT as u8 => Ok(CartridgeType::MBC3_TIMER_BATT),
            x if x == CartridgeType::MBC3_TIMER_RAM_BATT as u8 => Ok(CartridgeType::MBC3_TIMER_RAM_BATT),
            x if x == CartridgeType::MBC3 as u8 => Ok(CartridgeType::MBC3),
            x if x == CartridgeType::MBC3_RAM as u8 => Ok(CartridgeType::MBC3_RAM),
            x if x == CartridgeType::MBC3_RAM_BATT as u8 => Ok(CartridgeType::MBC3_RAM_BATT),
            x if x == CartridgeType::MBC5 as u8 => Ok(CartridgeType::MBC5),
            x if x == CartridgeType::MBC5_RAM as u8 => Ok(CartridgeType::MBC5_RAM),
            x if x == CartridgeType::MBC5_RAM_BATT as u8 => Ok(CartridgeType::MBC5_RAM_BATT),
            x if x == CartridgeType::MBC5_RUMBLE as u8 => Ok(CartridgeType::MBC5_RUMBLE),
            x if x == CartridgeType::MBC5_RUMBLE_RAM as u8 => Ok(CartridgeType::MBC5_RUMBLE_RAM),
            x if x == CartridgeType::MBC5_RUMBLE_RAM_BATT as u8 => Ok(CartridgeType::MBC5_RUMBLE_RAM_BATT),
            x if x == CartridgeType::POCKET_CAMERA as u8 => Ok(CartridgeType::POCKET_CAMERA),
            x if x == CartridgeType::BANDAI_TAMA5 as u8 => Ok(CartridgeType::BANDAI_TAMA5),
            x if x == CartridgeType::HuC3 as u8 => Ok(CartridgeType::HuC3),
            x if x == CartridgeType::HuC1_RAM_BATT as u8 => Ok(CartridgeType::HuC1_RAM_BATT),
            _ => Err(Error::InvalidValue(format!("Invalid CartridgeType: {}", val))),
        }
    }
}

pub struct Cartridge {
    /// Raw cartridge data
    pub(crate) data: Vec<u8>,
}

impl Cartridge {
    pub fn from_bytes(data: Vec<u8>) -> Self {
        Self {
            data,
        }
    }

    // Check if we're a valid cartridge
    pub fn validate(&self) -> Result<()> {
        if self.title().is_err() {
            return Err(CartridgeError::Title.into());
        }

        if self.manufacturer_code().is_err() {
            return Err(CartridgeError::ManufacturerCode.into());
        }

        if self.licensee_code().is_err() {
            return Err(CartridgeError::LicenseeCode.into());
        }

        if self.cartridge_type().is_err() {
            return Err(CartridgeError::Type.into());
        }

        if self.rom_size().is_err() {
            return Err(CartridgeError::RomSize.into());
        }

        if self.ram_size().is_err() {
            return Err(CartridgeError::RamSize.into());
        }

        Ok(())
    }

    /// Game title
    pub fn title(&self) -> Result<&str> {
        let raw = &self.data[0x134..0x143];
        Ok(std::str::from_utf8(raw)?)
    }

    /// Manufacturer code
    pub fn manufacturer_code(&self) -> Result<&str> {
        let raw = &self.data[0x13F..0x142];
        Ok(std::str::from_utf8(raw)?)
    }

    /// Licensee code (replaced manufacturer code on newer games)
    pub fn licensee_code(&self) -> Result<&str> {
        let raw = &self.data[0x144..0x145];
        let code: &str = std::str::from_utf8(raw)?;

        Ok(match code {
            "00" => "None",
            "01" => "Nintendo R&D 1",
            "31" => "Nintendo",
            _ => "Other",
        })
    }

    /// Cartridge type
    pub fn cartridge_type(&self) -> Result<CartridgeType> {
        CartridgeType::try_from(self.data[0x147])
    }

    /// ROM size
    pub fn rom_size(&self) -> Result<RomSize> {
        RomSize::try_from(self.data[0x148])
    }

    /// RAM size
    pub fn ram_size(&self) -> Result<RamSize> {
        RamSize::try_from(self.data[0x149])
    }
}