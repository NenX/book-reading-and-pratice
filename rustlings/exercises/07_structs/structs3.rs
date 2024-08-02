// Structs contain data, but can also have logic. In this exercise, we have
// defined the `Package` struct, and we want to test some logic attached to it.

#[derive(Debug)]
struct Package {
    sender_country: String,
    recipient_country: String,
    weight_in_grams: u32,
}

impl Package {
    fn new(sender_country: String, recipient_country: String, weight_in_grams: u32) -> Self {
        if weight_in_grams < 10 {
            // This isn't how you should handle errors in Rust, but we will
            // learn about error handling later.
            panic!("Can't ship a package with weight below 10 grams");
        }

        Self {
            sender_country,
            recipient_country,
            weight_in_grams,
        }
    }

    // TODO: Add the correct return type to the function signature.
    fn is_international(&self) -> bool {
        let Self {
            recipient_country,
            sender_country,
            weight_in_grams,
        } = self;
        [sender_country, recipient_country]
            .iter()
            .all(|x| *x != "Canada")
        // TODO: Read the tests that use this method to find out when a package
        // is considered international.
    }

    // TODO: Add the correct return type to the function signature.
    fn get_fees(&self, cents_per_gram: u32) -> u32 {
        self.weight_in_grams*cents_per_gram
        // TODO: Calculate the package's fees.
    }
}

fn main() {
    // You can optionally experiment here.
    let sender_country = String::from("Canada");
    let recipient_country = sender_country.clone();

    let package = Package::new(sender_country.clone(), recipient_country, 1200);
    let s = package.is_international();
    let s1 = sender_country == "Canada";
    println!("aa {package:?} {s} {s1}")
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic]
    fn fail_creating_weightless_package() {
        let sender_country = String::from("Spain");
        let recipient_country = String::from("Austria");

        Package::new(sender_country, recipient_country, 5);
    }

    #[test]
    fn create_international_package() {
        let sender_country = String::from("Spain");
        let recipient_country = String::from("Russia");

        let package = Package::new(sender_country, recipient_country, 1200);

        assert!(package.is_international());
    }

    #[test]
    fn create_local_package() {
        let sender_country = String::from("Canada");
        let recipient_country = sender_country.clone();

        let package = Package::new(sender_country, recipient_country, 1200);

        assert!(!package.is_international());
    }

    #[test]
    fn calculate_transport_fees() {
        let sender_country = String::from("Spain");
        let recipient_country = String::from("Spain");

        let cents_per_gram = 3;

        let package = Package::new(sender_country, recipient_country, 1500);

        assert_eq!(package.get_fees(cents_per_gram), 4500);
        assert_eq!(package.get_fees(cents_per_gram * 2), 9000);
    }
}
