use std::time::SystemTime;

use draw::{make_ball_joint, make_pipe_section, RenderObject};
use rand::Rng;
use three_d::{
    degrees, vec3, Camera, ClearState, DirectionalLight, FrameOutput, OrbitControl, Srgba, Window,
    WindowSettings,
};
use util::Coordinate;
use world::World;

mod draw;
mod pipe;
mod util;
mod world;

const MAX_PIPES: u32 = 10;

fn main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let mut world = World::new();
    let mut rng = rand::thread_rng();
    let init_data = world.new_pipe(&mut rng);

    //===============================================
    // ENGINE INIT
    //===============================================
    let window = Window::new(WindowSettings {
        title: "Rust Pipes".to_string(),
        max_size: Some((1280, 720)),
        ..Default::default()
    })
    .unwrap();
    let context = window.gl();

    let mut camera = Camera::new_perspective(
        window.viewport(),
        vec3(-20.0, 10.0, -20.0),
        vec3(0.0, 10.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        degrees(45.0),
        0.1,
        1000.0,
    );
    let mut control = OrbitControl::new(*camera.target(), 1.0, 100.0);

    // let axes = Axes::new(&context, 0.1, 2.0);
    let light0 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, -0.5, -0.5));
    let light1 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, 0.5, 0.5));

    let mut pipe_render_objs: [Vec<RenderObject>; 10] = Default::default();
    let mut last_seg_coords: [Coordinate; 10] = Default::default();
    last_seg_coords[0] = init_data.start_node;
    let start_time = SystemTime::now();

    window.render_loop(move |mut frame_input| {
        camera.set_viewport(frame_input.viewport);
        control.handle_events(&mut camera, &mut frame_input.events);

        //World Update Step
        if !world.is_gen_complete() {
            for i in 0..world.active_pipes_count() {
                if !world.is_pipe_alive(i) {
                    continue;
                }
                let delta_state = world.pipe_update(i, &mut rng);

                if delta_state.last_node != delta_state.current_node {
                    if delta_state.last_dir != delta_state.current_dir {
                        pipe_render_objs[i].push(make_ball_joint(
                            delta_state.last_node,
                            delta_state.pipe_color,
                            &context,
                        ));
                        pipe_render_objs[i].push(make_pipe_section(
                            delta_state.last_node,
                            delta_state.current_node,
                            delta_state.pipe_color,
                            &context,
                        ));
                        last_seg_coords[i] = delta_state.last_node;
                    } else {
                        pipe_render_objs[i].pop(); //reduce number of render objects in mem
                        pipe_render_objs[i].push(make_pipe_section(
                            last_seg_coords[i],
                            delta_state.current_node,
                            delta_state.pipe_color,
                            &context,
                        ));
                    }
                }
            }
        }
        if rng.gen_bool(world.new_pipe_chance()) && world.max_active_count_reached(MAX_PIPES) {
            let data = world.new_pipe(&mut rng);
            last_seg_coords[world.max_active_pipe_idx()] = data.start_node;
            pipe_render_objs[world.max_active_pipe_idx()].push(make_ball_joint(
                data.start_node,
                data.color,
                &context,
            ));
        }

        match start_time.elapsed() {
            Ok(elapsed) => {
                if elapsed.as_secs_f64() >= 15.0 {
                    world.set_gen_complete();
                    for i in 0..world.active_pipes_count() {
                        world.kill_pipe(i);
                    }
                }
            }
            Err(_) => panic!("Timer Did an oopsie, Panicking!!!!"),
        }

        frame_input
            .screen()
            .clear(ClearState::color_and_depth(0.8, 0.8, 0.8, 1.0, 1.0))
            .render(
                &camera,
                pipe_render_objs.iter().flatten(),
                &[&light0, &light1],
            );

        FrameOutput::default()
    });
}

#[cfg(test)]
mod tests {}
