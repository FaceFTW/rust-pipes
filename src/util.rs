use core::fmt;
use rand::Rng;
use std::{collections::HashSet, slice::Iter};
use three_d::{Deg, Mat4};

pub type Coordinate = (i32, i32, i32);
pub type Color = (u8, u8, u8);

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

impl Into<Mat4> for Direction {
    fn into(self) -> Mat4 {
        match self {
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

pub fn choose_random_direction(rng: &mut impl Rng) -> Direction {
    let dir = match rng.gen_range(0..6) {
        0 => Direction::North,
        1 => Direction::South,
        2 => Direction::East,
        3 => Direction::West,
        4 => Direction::Up,
        5 => Direction::Down,
        _ => panic!("Invalid direction generated"),
    };
    dir
}

/// Returns a random coordinate that is not occupied by any other pipe (including the pipe itself if
/// somehow the pipe is already on the board)
pub fn find_random_start(
    occupied_nodes: &HashSet<Coordinate>,
    bounds: Coordinate,
    rng: &mut impl Rng,
) -> Coordinate {
    //Double check if somehow there is no more space on the board
    if occupied_nodes.len() == (bounds.0 * bounds.1 * bounds.2) as usize {
        panic!("No more space on the board!");
    }

    let (x_max, y_max, z_max) = bounds;
    let mut coord = (
        rng.gen_range(0..x_max),
        rng.gen_range(0..y_max),
        rng.gen_range(0..z_max),
    );
    while occupied_nodes.contains(&coord) {
        coord = (
            rng.gen_range(0..x_max),
            rng.gen_range(0..y_max),
            rng.gen_range(0..z_max),
        );
    }
    coord
}

#[cfg(test)]
mod tests {
    use super::*;

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
