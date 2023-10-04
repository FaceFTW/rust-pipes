use draw::make_pipe_section;
use rand::Rng;
use std::{thread::sleep, time::Duration};
use three_d::{
    degrees, vec3, Axes, Camera, ClearState, DirectionalLight, FrameOutput, OrbitControl, Srgba,
    Window, WindowSettings,
};
use world::World;

mod draw;
mod pipe;
mod util;
mod world;

const MAX_PIPES: i32 = 10;

fn main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let mut world = World::new();
    let mut rng = rand::thread_rng();
    world.new_pipe(&mut rng);

    let window = Window::new(WindowSettings {
        title: "Rust Pipes".to_string(),
        max_size: Some((1280, 720)),
        ..Default::default()
    })
    .unwrap();
    let context = window.gl();

    let mut camera = Camera::new_perspective(
        window.viewport(),
        vec3(5.0, 2.0, 2.5),
        vec3(0.0, 0.0, -0.5),
        vec3(0.0, 1.0, 0.0),
        degrees(45.0),
        0.1,
        1000.0,
    );
    let mut control = OrbitControl::new(*camera.target(), 1.0, 100.0);

    let cylinder = make_pipe_section((0, -1, 0), (0, 0, 0), (0, 255, 0), &context);
    let cylinder2 = make_pipe_section((0, 0, 0), (1, 0, 0), (255, 0, 0), &context);

    let axes = Axes::new(&context, 0.1, 2.0);
    let light0 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, -0.5, -0.5));
    let light1 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, 0.5, 0.5));

    window.render_loop(move |mut frame_input| {
        camera.set_viewport(frame_input.viewport);
        control.handle_events(&mut camera, &mut frame_input.events);

        frame_input
            .screen()
            .clear(ClearState::color_and_depth(0.8, 0.8, 0.8, 1.0, 1.0))
            .render(
                &camera,
                axes.into_iter().chain(&*cylinder).chain(&*cylinder2),
                &[&light0, &light1],
            );

        FrameOutput::default()
    });
}

//OLD CODE - WILL BE DELETED
#[cfg(feature = "k3d_engine")]
fn k3d_run(world: &mut World, mut rng: impl Rng) {
    extern crate kiss3d;
    use draw::{draw_axes, make_ball_joint, make_pipe_section};
    use kiss3d::{camera::ArcBall, nalgebra::Point3, scene::SceneNode, window::Window};
    //===============================================
    // KISS3D INITIALIZATION
    //===============================================
    let mut window = Window::new("rust-pipes");

    let mut pipe_nodes: Vec<SceneNode> = Vec::new();
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
