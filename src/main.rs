use std::{thread::sleep, time::Duration};

use base::{
    draw::{draw_axes, make_ball_joint, make_pipe_section},
    world::World,
};
use kiss3d::{camera::ArcBall, nalgebra::Point3, scene::SceneNode, window::Window};
use rand::Rng;

extern crate kiss3d;

mod base;

const MAX_PIPES: i32 = 10;

fn main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let mut world = World::new();
    let mut pipe_nodes: Vec<SceneNode> = Vec::new();
    let mut rng = rand::thread_rng();
    world.new_pipe(&mut rng);

    //===============================================
    // KISS3D INITIALIZATION
    //===============================================
    let mut window = Window::new("rust-pipes");

    let eye = Point3::new(-20.0 as f32, 10.0, -20.0);
    let at_point = Point3::new(10.0 as f32, 10.0, 10.0);
    let mut camera = ArcBall::new(eye, at_point);

    while window.render_with_camera(&mut camera) {
        draw_axes(&mut window);
        sleep(Duration::from_millis(100));

        for i in 0..world.active_pipes {
            if !world.pipes[i].is_alive() {
                continue;
            }
            let delta_state = world.pipe_update(i, &mut rng);

            if delta_state.last_node != delta_state.current_node {
                if delta_state.last_dir != delta_state.current_dir {
                    pipe_nodes.push(make_ball_joint(
                        delta_state.last_node,
                        &mut window,
                        delta_state.pipe_color,
                    ));
                } else {
                    pipe_nodes.push(make_pipe_section(
                        delta_state.last_node,
                        delta_state.current_node,
                        &mut window,
                        delta_state.pipe_color,
                    ));
                }
            }
        }
        if rng.gen_bool(world.new_pipe_chance) && world.active_pipes < MAX_PIPES as usize {
            let data = world.new_pipe(&mut rng);
            pipe_nodes.push(make_ball_joint(data.start_node, &mut window, data.color));
        }
    }
}

#[cfg(test)]
mod tests {}
