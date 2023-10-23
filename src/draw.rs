use three_d::{Instances, Mat4, Srgba, Vec3};

use crate::util::Direction;

use super::util::{Color, Coordinate};

const PIPE_RADIUS: f32 = 0.15;
const BALL_JOINT_RADIUS: f32 = 0.3;

fn color_to_srgba(color: Color) -> Srgba {
    Srgba {
        r: color.0,
        g: color.1,
        b: color.2,
        a: 255,
    }
}

pub fn make_instanced_pipe_section(
    instance_struct: &mut Instances,
    from: Coordinate,
    to: Coordinate,
    color: Color,
) {
    let (from_x, from_y, from_z) = from;
    let (to_x, to_y, to_z) = to;
    let delta = (to_x - from_x, to_y - from_y, to_z - from_z);
    let pipe_len = ((delta.0.pow(2) + delta.1.pow(2) + delta.2.pow(2)) as f32).sqrt();

    let translation_matrix =
        Mat4::from_translation(Vec3::new(from_x as f32, from_y as f32, from_z as f32));
    let rotation_matrix: Mat4 = Direction::from(delta).into();
    let scale_matrix = Mat4::from_nonuniform_scale(pipe_len, PIPE_RADIUS, PIPE_RADIUS);
    let transform = translation_matrix * rotation_matrix * scale_matrix;

    instance_struct.transformations.push(transform);
    instance_struct
        .colors
        .as_mut()
        .unwrap_or(&mut Vec::<Srgba>::new())
        .push(color_to_srgba(color));
}

pub fn make_instanced_ball_joint(instance_struct: &mut Instances, pos: Coordinate, color: Color) {
    let (pos_x, pos_y, pos_z) = pos;

    let translation_matrix =
        Mat4::from_translation(Vec3::new(pos_x as f32, pos_y as f32, pos_z as f32));
    let scale_matrix = Mat4::from_scale(BALL_JOINT_RADIUS); //Scale is uniform here unlike pipes
    let transform = translation_matrix * scale_matrix;

    instance_struct.transformations.push(transform);
    instance_struct
        .colors
        .as_mut()
        .unwrap_or(&mut Vec::<Srgba>::new())
        .push(color_to_srgba(color));
}

#[cfg(test)]
mod tests {}
