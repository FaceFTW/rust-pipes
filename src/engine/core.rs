use std::borrow::Borrow;

///Defines core functions and structs used in the main engine loop
use three_d::{
    Context, CpuMaterial, CpuMesh, Gm, InstancedMesh, Instances, PhysicalMaterial, Srgba,
};
use three_d_asset::Mat4;

use super::rng::StdRng;
use super::teapot::teapot_mesh;
use super::util::{add_new_ball_joint, add_new_pipe_section};
use super::{config::Configuration, rng::EngineRng, util::InstantShim, world::World};

///Used to contain the information needed to render a `crate::world::Pipe`
pub struct RenderedPipe {
    ball_instances: Instances,
    pipe_instances: Instances,
    ball_geometry: Gm<InstancedMesh, PhysicalMaterial>,
    pipe_geometry: Gm<InstancedMesh, PhysicalMaterial>,
}

impl RenderedPipe {
    pub fn new(
        color: Srgba,
        cfg: &Configuration,
        context: &Context,
        pipe_mesh: &CpuMesh,
        joint_mesh: &CpuMesh,
    ) -> Self {
        //Instanced Rendering Data
        let pipe_instances = Instances {
            transformations: Vec::new(),
            ..Default::default()
        };
        let ball_instances = Instances {
            transformations: Vec::new(),
            ..Default::default()
        };

        let base_instance_material = CpuMaterial {
            albedo: color,
            ..Default::default()
        };

        let pipe_geometry = Gm::new(
            InstancedMesh::new(context, &Instances::default(), &pipe_mesh),
            PhysicalMaterial::new(&context, &base_instance_material),
        );
        let ball_geometry = Gm::new(
            InstancedMesh::new(&context, &Instances::default(), &joint_mesh),
            PhysicalMaterial::new(&context, &base_instance_material),
        );
        RenderedPipe {
            ball_instances,
            pipe_instances,
            ball_geometry,
            pipe_geometry,
        }
    }

    pub fn add_pipe_instance(&mut self, transform: Mat4) {
        self.pipe_instances.transformations.push(transform);
        self.pipe_geometry.set_instances(&self.pipe_instances);
    }

    pub fn add_ball_instance(&mut self, transform: Mat4) {
        self.ball_instances.transformations.push(transform);
        self.ball_geometry.set_instances(&self.ball_instances);
    }

    // pub fn reset_instances(&mut self) {
    //     self.ball_instances.transformations.clear();
    //     self.pipe_instances.transformations.clear();

    //     self.ball_geometry.set_instances(&self.ball_instances);
    //     self.pipe_geometry.set_instances(&self.pipe_instances);
    // }

    pub fn get_render_objects(
        &self,
    ) -> impl IntoIterator<Item = &Gm<InstancedMesh, PhysicalMaterial>> {
        [&self.ball_geometry, &self.pipe_geometry].into_iter()
    }
}

pub struct Engine {
    world: World,
    config: Box<Configuration>,
    gl_context: Box<Context>, //TODO will this cause issues??
    pipes: Vec<RenderedPipe>,
    rng: StdRng,
    //Reference meshes
    sphere_mesh: CpuMesh,
    pipe_mesh: CpuMesh,
    teapot_mesh: CpuMesh,
}

impl Engine {
    pub fn new<'c>(config: &Configuration, context: &Context) -> Self {
        let seed = config.rng_seed.borrow().to_owned(); //yucky
        let mut new_engine = Self {
            world: World::new(Some(&config)),
            config: Box::new(config.clone()),
            gl_context: Box::new(context.clone()),
            pipes: Vec::new(),
            rng: match seed {
                Some(val) => StdRng::with_seed(val),
                None => StdRng::new(),
            },
            sphere_mesh: CpuMesh::sphere(config.draw.angle_subdiv),
            pipe_mesh: CpuMesh::cylinder(config.draw.angle_subdiv),
            teapot_mesh: teapot_mesh(),
        };

        new_engine.init_new_pipe();

        new_engine
    }

    pub fn init_new_pipe(&mut self) {
        let data = self.world.new_pipe(&mut self.rng);
        let mut pipe = RenderedPipe::new(
            Srgba {
                r: self.rng.u8(u8::MIN, u8::MAX),
                g: self.rng.u8(u8::MIN, u8::MAX),
                b: self.rng.u8(u8::MIN, u8::MAX),
                a: u8::MAX,
            },
            &self.config,
            &self.gl_context,
            &self.pipe_mesh,
            &self.sphere_mesh,
        );
        add_new_ball_joint(&mut pipe, data.start_node);

        self.pipes.push(pipe);
    }

    pub fn update_pipe(&mut self, idx: usize) {
        if !self.world.is_pipe_alive(idx) {
            return;
        }
        let delta_state = self.world.pipe_update(idx, &mut self.rng);

        if delta_state.last_node != delta_state.current_node {
            if delta_state.last_dir != delta_state.current_dir {
                add_new_ball_joint(
                    self.pipes
                        .get_mut(idx)
                        .expect("RenderedPipe struct instance should exist here!!!"),
                    delta_state.last_node,
                )
            }
            add_new_pipe_section(
                self.pipes
                    .get_mut(idx)
                    .expect("RenderedPipe struct instance should exist here!!!"),
                delta_state.last_node,
                delta_state.current_node,
            )
        }
    }

    pub fn get_render_objects(
        &mut self,
    ) -> impl IntoIterator<Item = &Gm<InstancedMesh, PhysicalMaterial>> {
        self.pipes
            .as_slice()
            .into_iter()
            .flat_map(|pipe| pipe.get_render_objects())
    }

    pub fn update_state(&mut self, start_time: InstantShim) -> bool {
        if !self.world.is_gen_complete() {
            for i in 0..self.world.active_pipes_count() {
                self.update_pipe(i);
            }
        }

        if self.rng.bool(self.world.new_pipe_chance())
            && self
                .world
                .max_active_count_reached(self.config.world.max_pipes)
        {
            self.init_new_pipe();
        }

        check_time_step(start_time, &self.config, &mut self.world)
    }

    pub fn reset_state(&mut self) {
        self.world = World::new(Some(&self.config));
        self.pipes.clear();
    }
}

pub(crate) fn check_time_step(
    start_time: InstantShim,
    cfg: &Configuration,
    world: &mut World,
) -> bool {
    match start_time.elapsed() {
        elapsed if elapsed.as_secs_f64() < cfg.world.max_gen_time as f64 => (),
        elapsed
            if elapsed.as_secs_f64() >= cfg.world.max_gen_time as f64
                && elapsed.as_secs_f64() < cfg.world.max_freeze_time as f64 =>
        {
            world.set_gen_complete();
            for i in 0..world.active_pipes_count() {
                world.kill_pipe(i);
            }
        }
        elapsed
            if elapsed.as_secs_f64()
                >= (cfg.world.max_gen_time + cfg.world.max_freeze_time) as f64
                && elapsed.as_secs_f64() < cfg.world.max_cycle_time =>
        {
            ()
        }
        _ => return true,
    }
    false
}
