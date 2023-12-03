use super::util::{Color, Coordinate, Direction};
use three_d::{Instances, Mat4, Srgba, Vec3};

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
        .expect("Instance Struct should have Color Instance Vector!")
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
        .expect("Instance Struct should have Color Instances Vector!")
        .push(color_to_srgba(color));
}

#[cfg(test)]
mod tests {
    use three_d::{Instances, Mat4, Vec3};

    use crate::{
        draw::{make_instanced_ball_joint, make_instanced_pipe_section, BALL_JOINT_RADIUS},
        util::Direction,
    };

    use super::{color_to_srgba, PIPE_RADIUS};

    #[test]
    pub fn make_instanced_pipe_section_creates_expected_instances() {
        let mut pipe_instances = Instances {
            transformations: Vec::new(),
            colors: Some(Vec::new()),
            ..Default::default()
        };

        let scale_mat = Mat4::from_nonuniform_scale(1.0, PIPE_RADIUS, PIPE_RADIUS);

        let rot_mat_north: Mat4 = Direction::North.into();
        let rot_mat_south: Mat4 = Direction::South.into();
        let rot_mat_east: Mat4 = Direction::East.into();
        let rot_mat_west: Mat4 = Direction::West.into();
        let rot_mat_up: Mat4 = Direction::Up.into();
        let rot_mat_down: Mat4 = Direction::Down.into();

        let expected_transforms = vec![
            Mat4::from_translation(Vec3::new(0.0, 0.0, 0.0)) * rot_mat_north * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 0.0, 0.0)) * rot_mat_south * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 1.0, 0.0)) * rot_mat_east * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 0.0, 1.0)) * rot_mat_west * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 1.0, 0.0)) * rot_mat_up * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 0.0, 1.0)) * rot_mat_down * scale_mat,
        ];

        let expected_colors = vec![
            color_to_srgba((0, 0, 0)),
            color_to_srgba((255, 0, 0)),
            color_to_srgba((0, 255, 0)),
            color_to_srgba((0, 0, 255)),
            color_to_srgba((255, 255, 0)),
            color_to_srgba((255, 255, 255)),
        ];

        make_instanced_pipe_section(&mut pipe_instances, (0, 0, 0), (1, 0, 0), (0, 0, 0));
        make_instanced_pipe_section(&mut pipe_instances, (1, 0, 0), (0, 0, 0), (255, 0, 0));
        make_instanced_pipe_section(&mut pipe_instances, (0, 1, 0), (0, 1, 1), (0, 255, 0));
        make_instanced_pipe_section(&mut pipe_instances, (0, 0, 1), (0, 0, 0), (0, 0, 255));
        make_instanced_pipe_section(&mut pipe_instances, (1, 1, 0), (1, 2, 0), (255, 255, 0));
        make_instanced_pipe_section(&mut pipe_instances, (1, 0, 1), (1, -1, 1), (255, 255, 255));

        assert_eq!(
            pipe_instances.transformations.len(),
            expected_transforms.len()
        );

        assert!(pipe_instances.colors.is_some());
        let actual_colors = pipe_instances
            .colors
            .expect("Colors Vector was not found in Instance Struct!");

        assert_eq!(actual_colors.len(), expected_colors.len());

        for i in 0..pipe_instances.transformations.len() {
            let expected_transform = expected_transforms[i];
            let actual_transform = pipe_instances.transformations[i];
            assert_eq!(expected_transform, actual_transform);

            let expected_color = expected_colors[i];
            let actual_color = actual_colors[i];
            assert_eq!(expected_color, actual_color);
        }
    }

    #[test]
    pub fn make_instanced_ball_joint_creates_expected_instances() {
        let mut ball_instances = Instances {
            transformations: Vec::new(),
            colors: Some(Vec::new()),
            ..Default::default()
        };

        let scale_mat = Mat4::from_scale(BALL_JOINT_RADIUS);

        let expected_transforms = vec![
            Mat4::from_translation(Vec3::new(0.0, 0.0, 0.0)) * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 0.0, 0.0)) * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 1.0, 0.0)) * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 0.0, 1.0)) * scale_mat,
        ];

        let expected_colors = vec![
            color_to_srgba((0, 0, 0)),
            color_to_srgba((255, 0, 0)),
            color_to_srgba((0, 255, 0)),
            color_to_srgba((0, 0, 255)),
        ];

        make_instanced_ball_joint(&mut ball_instances, (0, 0, 0), (0, 0, 0));
        make_instanced_ball_joint(&mut ball_instances, (1, 0, 0), (255, 0, 0));
        make_instanced_ball_joint(&mut ball_instances, (0, 1, 0), (0, 255, 0));
        make_instanced_ball_joint(&mut ball_instances, (0, 0, 1), (0, 0, 255));

        assert_eq!(
            ball_instances.transformations.len(),
            expected_transforms.len()
        );

        assert!(ball_instances.colors.is_some());
        let actual_colors = ball_instances
            .colors
            .expect("Colors Vector was not found in Instance Struct!");

        assert_eq!(actual_colors.len(), expected_colors.len());

        for i in 0..ball_instances.transformations.len() {
            let expected_transform = expected_transforms[i];
            let actual_transform = ball_instances.transformations[i];
            assert_eq!(expected_transform, actual_transform);

            let expected_color = expected_colors[i];
            let actual_color = actual_colors[i];
            assert_eq!(expected_color, actual_color);
        }
    }
}
