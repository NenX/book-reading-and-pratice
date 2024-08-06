use std::{sync::{Arc, Mutex}, thread, vec};



#[test]
fn test() {
    let counter = Arc::new(Mutex::new(vec![(0,0)]));
    let mut handlers = vec![];
    for i in 0..10 {

        let counter = Arc::clone(&counter);
        
        let h = thread::spawn(move||{

            let mut x = counter.lock().unwrap();
            for ni in 0..5 {
              
                x.push((i,ni))
            }
        });
    
        handlers.push(h);

    }

    // for i in &handlers {
    //     i.join().unwrap();
    // }
    for i in handlers {
        i.join().unwrap();
    }
    let a = counter.lock().unwrap();
    println!(" --> {a:?}") // ling

}