mod base;
use base::pipe::PipeManager;
use base::util::Coordinate;

fn main() {
    let mut manager = PipeManager::new((10, 10, 10));
    manager.generate_pipe();
    manager.generate_pipe();

    let pipe_iter: [&Vec<Coordinate>; 2] =
        [manager.pipe(0).get_nodes(), manager.pipe(1).get_nodes()];

    for i in 0..2 {
        println!("Pipe {}", i);
        for coord in pipe_iter[i] {
            println!("Node: {:?}", coord);
        }
    }
}

#[cfg(test)]
mod tests {}
