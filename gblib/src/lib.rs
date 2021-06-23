pub mod cartridge;
pub mod error;
mod memory;
mod registers;

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
