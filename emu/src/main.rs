use std::path::PathBuf;

use structopt::StructOpt;

use gblib::cartridge::Cartridge;

#[derive(Debug, StructOpt)]
#[structopt(about = "A Game Boy emulator written in Rust")]
enum Args {
    #[structopt(about = "Inspect one or more ROM files")]
    Inspect {
        #[structopt(parse(from_os_str))]
        rom_file: Vec<PathBuf>
    }
}

fn get_cartridge(path: &PathBuf) -> Cartridge {
    let data = std::fs::read(path).expect("Failed to open ROM file");
    let cartridge = Cartridge::from_bytes(data);
    cartridge
}

fn main() {
    env_logger::init();

    let cli = Args::from_args();

    match cli {
        Args::Inspect { rom_file } => {
            for f in &rom_file {
                let cartridge = get_cartridge(f);

                println!("\nTitle: {}", cartridge.title().unwrap_or("N/A"));
                println!("\nManufacturer Code: {}", cartridge.manufacturer_code().unwrap_or("N/A"));
                println!("\nLicensee Code: {}", cartridge.licensee_code().unwrap_or("N/A"));
                println!("\nCartridge Type: {:?}", cartridge.cartridge_type().unwrap());
                println!("\nROM Size: {:?}", cartridge.rom_size().unwrap());
                println!("\nRAM Size: {:?}", cartridge.ram_size().unwrap());

            }
        }
    }
}