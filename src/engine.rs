mod config;
mod util;
mod world;

use crate::engine::{
    config::Configuration,
    util::{make_instanced_ball_joint, make_instanced_pipe_section, RngShim, InstantShim},
    world::World,
};
use cfg_if::cfg_if;
use fastrand::Rng;
use three_d::{
    degrees, vec3, Camera, ClearState, CpuMaterial, CpuMesh, DirectionalLight, FrameOutput, Gm,
    InstancedMesh, Instances, OrbitControl, PhysicalMaterial, Srgba, Window, WindowSettings,
};

pub fn real_main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let cfg = get_config();
    let mut world = World::new(Some(&cfg));
    let mut rng = match cfg.rng_seed {
        Some(seed) => Rng::with_seed(seed),
        None => Rng::new(),
    };
    dbg!(rng.get_seed());

    //===============================================
    // ENGINE INIT
    //===============================================
    let window = Window::new(WindowSettings {
        title: "Rust Pipes".to_string(),
        max_size: cfg.window_size,
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

    let light0 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, -0.5, -0.5));
    let light1 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, 0.5, 0.5));

    //Instanced Rendering Data
    let mut pipe_instances = Instances {
        transformations: Vec::new(),
        colors: Some(Vec::new()),
        ..Default::default()
    };
    let mut ball_instances = Instances {
        transformations: Vec::new(),
        colors: Some(Vec::new()),
        ..Default::default()
    };

    let base_instance_material = CpuMaterial {
        albedo: Srgba {
            r: 255,
            g: 255,
            b: 255,
            a: 255,
        },
        ..Default::default()
    };

    let mut pipe_instance_mesh = Gm::new(
        InstancedMesh::new(
            &context,
            &Instances::default(),
            &CpuMesh::cylinder(cfg.draw.angle_subdiv),
        ),
        PhysicalMaterial::new(&context, &base_instance_material),
    );
    let mut ball_instance_mesh = Gm::new(
        InstancedMesh::new(
            &context,
            &Instances::default(),
            &CpuMesh::sphere(cfg.draw.angle_subdiv),
        ),
        PhysicalMaterial::new(&context, &base_instance_material),
    );

    let mut start_time = InstantShim::now();
    let mut do_reset = false;

    window.render_loop(move |mut frame_input| {
        camera.set_viewport(frame_input.viewport);
        control.handle_events(&mut camera, &mut frame_input.events);

        if do_reset && !cfg.single_run {
            start_time = InstantShim::now();
            world = World::new(Some(&cfg));
            pipe_instances = Instances {
                transformations: Vec::new(),
                colors: Some(Vec::new()),
                ..Default::default()
            };
            ball_instances = Instances {
                transformations: Vec::new(),
                colors: Some(Vec::new()),
                ..Default::default()
            };
            // do_reset = false;
        }

        //World Update Step
        do_reset = world_update_tick(
            &mut world,
            &mut rng,
            &mut ball_instances,
            &mut pipe_instances,
            start_time,
            &cfg,
        );

        pipe_instance_mesh.set_instances(&pipe_instances);
        ball_instance_mesh.set_instances(&ball_instances);
        frame_input
            .screen()
            .clear(ClearState::color_and_depth(
                cfg.draw.bg_color.0 as f32 / 255.0,
                cfg.draw.bg_color.1 as f32 / 255.0,
                cfg.draw.bg_color.2 as f32 / 255.0,
                1.0,
                1.0,
            ))
            .render(
                &camera,
                [&pipe_instance_mesh, &ball_instance_mesh].into_iter(),
                &[&light0, &light1],
            );

        FrameOutput::default()
    });
}

cfg_if! {
  if #[cfg(target_arch="wasm32")]{
    fn get_config() -> Configuration{
        Configuration::new()
    }
  } else {
    use crate::engine::config::make_cli_parser;
    fn get_config() -> Configuration{
        let cli_args = &make_cli_parser().get_matches();
        // dbg!(cli_args);
        Configuration::new(cli_args)
    }
  }
}

fn world_update_tick(
    world: &mut World,
    mut rng: &mut Rng,
    ball_instances: &mut Instances,
    pipe_instances: &mut Instances,
    start_time: InstantShim,
    cfg: &Configuration,
) -> bool {
    if !world.is_gen_complete() {
        for i in 0..world.active_pipes_count() {
            if !world.is_pipe_alive(i) {
                continue;
            }
            let delta_state = world.pipe_update(i, &mut rng);

            if delta_state.last_node != delta_state.current_node {
                if delta_state.last_dir != delta_state.current_dir {
                    make_instanced_ball_joint(
                        ball_instances,
                        delta_state.last_node,
                        delta_state.pipe_color,
                    );
                }
                make_instanced_pipe_section(
                    pipe_instances,
                    delta_state.last_node,
                    delta_state.current_node,
                    delta_state.pipe_color,
                );
            }
        }
    }
    if rng.gen_bool(world.new_pipe_chance()) && world.max_active_count_reached(cfg.world.max_pipes) {
        let data = world.new_pipe(&mut rng);
        make_instanced_ball_joint(ball_instances, data.start_node, data.color);
    }

    let max_cycle_time = (cfg.world.max_gen_time + cfg.world.max_freeze_time) as f64;

    match start_time.elapsed() {
        elapsed if elapsed.as_secs_f64() < cfg.world.max_gen_time as f64 => (),
        elapsed
            if elapsed.as_secs_f64() >= cfg.world.max_gen_time as f64
                && elapsed.as_secs_f64() < max_cycle_time =>
        {
            world.set_gen_complete();
            for i in 0..world.active_pipes_count() {
                world.kill_pipe(i);
            }
        }
        _ => return true,
    }
    false
}


