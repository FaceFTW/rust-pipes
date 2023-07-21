use core::fmt;
use std::slice::Iter;

pub type Coordinate = (i32, i32, i32);

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
        Direction::North => (x, y + 1, z),
        Direction::South => (x, y - 1, z),
        Direction::East => (x + 1, y, z),
        Direction::West => (x - 1, y, z),
        Direction::Up => (x, y, z + 1),
        Direction::Down => (x, y, z - 1),
    }
}

pub fn get_neighbors(coord: Coordinate) -> Vec<Coordinate> {
    let mut neighbors = Vec::new();
    for dir in [
        Direction::North,
        Direction::South,
        Direction::East,
        Direction::West,
        Direction::Up,
        Direction::Down,
    ]
    .iter()
    {
        neighbors.push(step_in_dir(coord, *dir));
    }
    neighbors
}

#[cfg(test)]
mod tests {
    use super::*;

    //could be broken down into multiple tests, but I'm lazy
    #[test]
    fn test_step_in_dir() {
        assert_eq!(step_in_dir((0, 0, 0), Direction::North), (0, 1, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::South), (0, -1, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::East), (1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::West), (-1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Up), (0, 0, 1));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Down), (0, 0, -1));
    }

	#[test]
	fn test_dir_iterator(){
		let mut dir_iter = Direction::iterator();
		assert_eq!(dir_iter.next(), Some(&Direction::North));
		assert_eq!(dir_iter.next(), Some(&Direction::South));
		assert_eq!(dir_iter.next(), Some(&Direction::East));
		assert_eq!(dir_iter.next(), Some(&Direction::West));
		assert_eq!(dir_iter.next(), Some(&Direction::Up));
		assert_eq!(dir_iter.next(), Some(&Direction::Down));
		assert_eq!(dir_iter.next(), None);
	}
}
