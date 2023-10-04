use three_d::{Context, CpuMaterial, CpuMesh, Gm, Mat4, Mesh, PhysicalMaterial, Srgba, Vec3};

use crate::util::Direction;

use super::util::{Color, Coordinate};

pub type RenderObject = Gm<Mesh, PhysicalMaterial>;

const PIPE_RADIUS: f32 = 0.15;
const BALL_JOINT_RADIUS: f32 = 0.3;

const DEFAULT_SUBDIVISIONS: u32 = 16;

fn gen_scale_transform(dir: Direction) -> Mat4 {
    match dir {
        Direction::North => Mat4::from_nonuniform_scale(1.0, PIPE_RADIUS, PIPE_RADIUS),
        Direction::South => Mat4::from_nonuniform_scale(1.0, PIPE_RADIUS, PIPE_RADIUS),
        Direction::East => Mat4::from_nonuniform_scale(PIPE_RADIUS, PIPE_RADIUS, 1.0),
        Direction::West => Mat4::from_nonuniform_scale(PIPE_RADIUS, PIPE_RADIUS, 1.0),
        Direction::Up => Mat4::from_nonuniform_scale(PIPE_RADIUS, 1.0, PIPE_RADIUS),
        Direction::Down => Mat4::from_nonuniform_scale(PIPE_RADIUS, 1.0, PIPE_RADIUS),
    }
}

fn color_to_srgba(color: Color) -> Srgba {
    Srgba {
        r: color.0,
        g: color.1,
        b: color.2,
        a: 255,
    }
}

pub fn make_pipe_section(
    from: Coordinate,
    to: Coordinate,
    color: Color,
    context: &Context,
) -> RenderObject {
    let (from_x, from_y, from_z) = from;
    let (to_x, to_y, to_z) = to;
    let delta = (to_x - from_x, to_y - from_y, to_z - from_z);

    let mesh = Mesh::new(context, &CpuMesh::cylinder(DEFAULT_SUBDIVISIONS));
    let material = PhysicalMaterial::new_opaque(
        context,
        &CpuMaterial {
            albedo: color_to_srgba(color),
            ..Default::default()
        },
    );
    let mut obj = Gm::new(mesh, material);

    let translation_matrix =
        Mat4::from_translation(Vec3::new(from_x as f32, from_y as f32, from_z as f32));
    let rotation_matrix: Mat4 = Direction::from(delta).into();
    let scale_matrix = gen_scale_transform(Direction::from(delta));
    let transform = scale_matrix * translation_matrix * rotation_matrix;

    obj.set_transformation(transform);

    obj
}

pub fn make_ball_joint(pos: Coordinate, color: Color, context: &Context) -> RenderObject {
    let (pos_x, pos_y, pos_z) = pos;

    let mesh = Mesh::new(context, &CpuMesh::sphere(DEFAULT_SUBDIVISIONS));
    let material = PhysicalMaterial::new_opaque(
        context,
        &CpuMaterial {
            albedo: color_to_srgba(color),
            ..Default::default()
        },
    );
    let mut obj = Gm::new(mesh, material);

    let translation_matrix =
        Mat4::from_translation(Vec3::new(pos_x as f32, pos_y as f32, pos_z as f32));
    let scale_matrix = Mat4::from_scale(BALL_JOINT_RADIUS); //Scale is uniform here unlike pipes
    let transform = scale_matrix * translation_matrix;

    obj.set_transformation(transform);

    obj
}

#[cfg(test)]
mod tests {}
