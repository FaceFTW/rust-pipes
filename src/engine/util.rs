use core::fmt;
// use rand::Rng;
use fastrand::Rng;
use std::{collections::HashSet, slice::Iter};
use three_d::{Deg, Instances, Mat4, Srgba, Vec3};

pub type Coordinate = (i32, i32, i32);
pub type Color = (u8, u8, u8);
// pub type F32Color = (f32, f32, f32);

const PIPE_RADIUS: f32 = 0.15;
const BALL_JOINT_RADIUS: f32 = 0.3;

//=============================================
// Random-related Utilities
//=============================================
///"shim" trait to allow for code practices that were
/// used with the original `rand` package in the project.
/// Mainly compatibility related, may no longer exist
/// when better ideas come to mind
pub(super) trait RngShim {
    fn gen_bool(&mut self, cutoff: f64) -> bool;
    fn gen(&mut self) -> (u8, u8, u8);
}

impl RngShim for Rng {
    fn gen_bool(&mut self, cutoff: f64) -> bool {
        self.f64() <= cutoff
    }

    fn gen(&mut self) -> (u8, u8, u8) {
        (
            self.u8(u8::MIN..u8::MAX),
            self.u8(u8::MIN..u8::MAX),
            self.u8(u8::MIN..u8::MAX),
        )
    }
}

//=============================================
// State-related Utilities
//=============================================
///Defines a direction in space
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Direction {
    North,
    South,
    East,
    West,
    Up,
    Down,
}

impl Direction {
    pub fn iterator() -> Iter<'static, Direction> {
        [
            Direction::North,
            Direction::South,
            Direction::East,
            Direction::West,
            Direction::Up,
            Direction::Down,
        ]
        .iter()
    }
}

impl From<Direction> for Mat4 {
    fn from(val: Direction) -> Self {
        match val {
            Direction::North => Mat4::from_angle_y(Deg(0.0)),
            Direction::South => Mat4::from_angle_y(Deg(180.0)),
            Direction::East => Mat4::from_angle_y(Deg(-90.0)),
            Direction::West => Mat4::from_angle_y(Deg(90.0)),
            Direction::Up => Mat4::from_angle_z(Deg(90.0)),
            Direction::Down => Mat4::from_angle_z(Deg(270.0)),
        }
    }
}

impl From<Coordinate> for Direction {
    fn from(value: Coordinate) -> Self {
        let (x, y, z) = value;
        match (x, y, z) {
            (x, 0, 0) if x > 0 => Direction::North,
            (x, 0, 0) if x < 0 => Direction::South,
            (0, y, 0) if y > 0 => Direction::Up,
            (0, y, 0) if y < 0 => Direction::Down,
            (0, 0, z) if z > 0 => Direction::East,
            (0, 0, z) if z < 0 => Direction::West,
            _ => panic!(
                "Could not convert coordinate/distance to a cardinal direction, {:?}",
                value
            ),
        }
    }
}

impl fmt::Display for Direction {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let dir_str = match self {
            Direction::North => "North",
            Direction::South => "South",
            Direction::East => "East",
            Direction::West => "West",
            Direction::Up => "Up",
            Direction::Down => "Down",
        };
        write!(f, "{}", dir_str)
    }
}

// pub fn convert_to_float_color(val: Color) -> F32Color {
//     (
//         val.0 as f32 / u8::MAX as f32,
//         val.1 as f32 / u8::MAX as f32,
//         val.2 as f32 / u8::MAX as f32,
//     )
// }

/// Steps a coordinate in a specified direction.
pub fn step_in_dir(coord: Coordinate, dir: Direction) -> Coordinate {
    let (x, y, z) = coord;
    match dir {
        Direction::North => (x + 1, y, z),
        Direction::South => (x - 1, y, z),
        Direction::East => (x, y, z + 1),
        Direction::West => (x, y, z - 1),
        Direction::Up => (x, y + 1, z),
        Direction::Down => (x, y - 1, z),
    }
}

pub fn is_in_bounds(coord: Coordinate, bounds: Coordinate) -> bool {
    let (x, y, z) = coord;
    let (x_max, y_max, z_max) = bounds;
    x >= 0 && x < x_max && y >= 0 && y < y_max && z >= 0 && z < z_max
}

pub fn choose_random_direction(rng: &mut Rng) -> Direction {
    match rng.u8(0..6) {
        0 => Direction::North,
        1 => Direction::South,
        2 => Direction::East,
        3 => Direction::West,
        4 => Direction::Up,
        5 => Direction::Down,
        _ => panic!("Invalid direction generated"),
    }
}

/// Returns a random coordinate that is not occupied by any other pipe (including the pipe itself if
/// somehow the pipe is already on the board)
pub fn find_random_start(
    occupied_nodes: &HashSet<Coordinate>,
    bounds: Coordinate,
    rng: &mut Rng,
) -> Coordinate {
    //Double check if somehow there is no more space on the board
    if occupied_nodes.len() == (bounds.0 * bounds.1 * bounds.2) as usize {
        panic!("No more space on the board!");
    }

    let (x_max, y_max, z_max) = bounds;
    let mut coord = (rng.i32(0..x_max), rng.i32(0..y_max), rng.i32(0..z_max));
    while occupied_nodes.contains(&coord) {
        coord = (rng.i32(0..x_max), rng.i32(0..y_max), rng.i32(0..z_max));
    }
    coord
}
fn color_to_srgba(color: Color) -> Srgba {
    Srgba {
        r: color.0,
        g: color.1,
        b: color.2,
        a: 255,
    }
}

//=============================================
// Rendering Related Utilities
//=============================================
///Creates an instance of a pipe segment with the required transformations.
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

//=============================================
// Unit Tests
//=============================================
#[cfg(test)]
mod tests {
    use super::*;
    use super::{color_to_srgba, PIPE_RADIUS};
    use three_d::{Instances, Mat4, Vec3};

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

    #[test]
    fn test_step_in_dir() {
        assert_eq!(step_in_dir((0, 0, 0), Direction::North), (1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::South), (-1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::East), (0, 0, 1));
        assert_eq!(step_in_dir((0, 0, 0), Direction::West), (0, 0, -1));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Up), (0, 1, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Down), (0, -1, 0));
    }

    #[test]
    fn test_dir_iterator() {
        let mut dir_iter = Direction::iterator();
        assert_eq!(dir_iter.next(), Some(&Direction::North));
        assert_eq!(dir_iter.next(), Some(&Direction::South));
        assert_eq!(dir_iter.next(), Some(&Direction::East));
        assert_eq!(dir_iter.next(), Some(&Direction::West));
        assert_eq!(dir_iter.next(), Some(&Direction::Up));
        assert_eq!(dir_iter.next(), Some(&Direction::Down));
        assert_eq!(dir_iter.next(), None);
    }

    #[test]
    fn test_direction_from_coordinate_cardinal_directions() {
        assert_eq!(Direction::from((1, 0, 0)), Direction::North);
        assert_eq!(Direction::from((-1, 0, 0)), Direction::South);
        assert_eq!(Direction::from((0, 1, 0)), Direction::Up);
        assert_eq!(Direction::from((0, -1, 0)), Direction::Down);
        assert_eq!(Direction::from((0, 0, 1)), Direction::East);
        assert_eq!(Direction::from((0, 0, -1)), Direction::West);
    }

    #[test]
    #[should_panic]
    fn test_direction_from_coordinate_vector_dirs_panic() {
        let _test2 = Direction::from((1, 2, 3));
    }

    #[test]
    #[should_panic]
    fn test_direction_from_coordinate_zero_vector_panics() {
        let _test1 = Direction::from((0, 0, 0));
    }
}
