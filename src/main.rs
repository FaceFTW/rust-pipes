extern crate kiss3d;

mod base;

use base::{pipe::PipeManager, util::Coordinate};
use kiss3d::camera::ArcBall;
use kiss3d::nalgebra::{Point3, Translation3, UnitQuaternion, Vector3};
use kiss3d::window::Window;

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

    //Draw Code
    let mut window = Window::new("rust-pipes");
    let eye = Point3::new(20.0 as f32, 20.0, 20.0);
    let at_point = Point3::new(0.0 as f32, 0.0, 0.0);
    let mut camera = ArcBall::new(eye, at_point);
    let mut objects: Vec<kiss3d::scene::SceneNode> = Vec::new();
    for pipe in manager.get_pipes() {
        for node in pipe.get_nodes() {
            let mut object = window.add_cube(1.0, 1.0, 1.0);
            object.set_color(1.0, 0.0, 0.0);
            object.append_translation(&Translation3::new(
                node.0 as f32,
                node.1 as f32,
                node.2 as f32,
            ));
            objects.push(object);
        }
    }

    while window.render_with_camera(&mut camera) {
        for object in &mut objects {
            object.prepend_to_local_rotation(&UnitQuaternion::from_axis_angle(
                &Vector3::y_axis(),
                0.014,
            ));
        }
    }
}

#[cfg(test)]
mod tests {}
