use std::collections::HashSet;

use base::{pipe::Pipe, util::Coordinate};
use kiss3d::{window::Window, nalgebra::Point3, camera::ArcBall};

extern crate kiss3d;

mod base;

// use base::{pipe::PipeManager, util::Coordinate};
// use kiss3d::camera::ArcBall;
// use kiss3d::nalgebra::{Point3, Translation3, UnitQuaternion, Vector3};
// use kiss3d::window::Window;
// use rand::Rng;

struct World {
    pipes: Vec<Pipe>,
    occupied_nodes: HashSet<Coordinate>,
    space_bounds: Coordinate,
	new_pipe_chance: f32,
}

impl Default for World {
	fn default() -> Self {
		World {
			pipes: Vec::new(),
			occupied_nodes: HashSet::new(),
			space_bounds: (50, 50, 50),
			new_pipe_chance: 0.5,
		}
	}
}

fn main() {
    let mut world = World {
		pipes: Vec::new(),
		occupied_nodes: HashSet::new(),
		space_bounds: (50, 50, 50),
		..Default::default()
    };
    let mut rng = rand::thread_rng();



    // let pipe_iter: [&Vec<Coordinate>; 2] =
    //     [manager.pipe(0).get_nodes(), manager.pipe(1).get_nodes()];

    // for i in 0..2 {
    //     println!("Pipe {}", i);
    //     for coord in pipe_iter[i] {
    //         println!("Node: {:?}", coord);
    //     }
    // }

    // //Draw Code
    let mut window = Window::new("rust-pipes");
    let eye = Point3::new(-20.0 as f32, -20.0, -20.0);
    let at_point = Point3::new(0.0 as f32, 0.0, 0.0);
    let mut camera = ArcBall::new(eye, at_point);
    // let mut objects: Vec<kiss3d::scene::SceneNode> = Vec::new();
    // for pipe in manager.get_pipes() {
    //     let pipe_red = rng.gen_range(0.0..=1.0);
    //     let pipe_green = rng.gen_range(0.0..=1.0);
    //     let pipe_blue = rng.gen_range(0.0..=1.0);

    //     for node in pipe.get_nodes() {
    //         // let mut object = window.add_cube(1.0, 1.0, 1.0);
    // 		let mut object = window.add_cylinder(0.5, 0.5);
    //         object.set_color(pipe_red, pipe_green, pipe_blue);
    //         object.append_translation(&Translation3::new(
    //             node.0 as f32,
    //             node.1 as f32,
    //             node.2 as f32,
    //         ));
    //         objects.push(object);
    //     }
    //     for sphere_point in pipe.get_sphere_points() {
    //         let mut sphere_obj = window.add_sphere(0.5);
    //         sphere_obj.set_color(pipe_red, pipe_green, pipe_blue);
    //         sphere_obj.append_translation(&Translation3::new(
    //             sphere_point.0 as f32,
    //             sphere_point.1 as f32,
    //             sphere_point.2 as f32,
    //         ));
    //         objects.push(sphere_obj);
    //     }
    // }

    // while window.render_with_camera(&mut camera) {
    //     for object in &mut objects {
    //         object.prepend_to_local_rotation(&UnitQuaternion::from_axis_angle(
    //             &Vector3::y_axis(),
    //             0.014,
    //         ));
    //     }
    // }
}

#[cfg(test)]
mod tests {}
