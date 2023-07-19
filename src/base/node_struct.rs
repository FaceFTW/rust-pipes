use super::util::*;
use rand::Rng;

pub struct NodeStruct {
    pub size: Coordinate,
    pub array: Vec<bool>,
}

impl NodeStruct {
    pub fn new(size: Coordinate) -> NodeStruct {
        if size.0 <= 0 || size.1 <= 0 || size.2 <= 0 {
            panic!(
                "Attempting to create a NodeStruct with negative/zero size: {:?}",
                size
            );
        }
        let vec_size = size.0 * size.1 * size.2;
        let mut vec = Vec::with_capacity(vec_size as usize);
        for _ in 0..vec_size {
            vec.push(false);
        }
        NodeStruct { size, array: vec }
    }

    fn to_vec_index(&self, coord: Coordinate) -> usize {
        let (x, y, z) = coord;
        let (x_size, y_size, z_size) = self.size;
        if x < 0 || y < 0 || z < 0 || x >= x_size || y >= y_size || z >= z_size {
            panic!(
                "Attempting to access coordinate out of bounds: {:?}, this is of size {:?}",
                coord, self.size
            );
        }
        (x * y_size * z_size + y * z_size + z) as usize
    }

    pub fn get(&self, coord: Coordinate) -> bool {
        self.array[self.to_vec_index(coord)]
    }

    pub fn set(&mut self, coord: Coordinate, val: bool) {
        let idx = self.to_vec_index(coord);
        self.array[idx] = val;
    }

    pub fn count_available_in_direction(&self, coord: Coordinate, dir: Direction) -> i32 {
        let mut count = 0;
        let mut current_coord = coord;
        let max_walk = match dir {
            Direction::North => self.size.1 - coord.1 - 1,
            Direction::South => coord.1,
            Direction::East => self.size.0 - coord.0 - 1,
            Direction::West => coord.0,
            Direction::Up => self.size.2 - coord.2 - 1,
            Direction::Down => coord.2,
        };

        while !self.get(current_coord) && count < max_walk {
            count += 1;
            current_coord = step_in_dir(current_coord, dir);
        }
        count
    }

    pub fn find_random_empty_node(&self) -> Coordinate {
        let mut rng = rand::thread_rng();
        let mut coord = (
            rng.gen_range(0..self.size.0),
            rng.gen_range(0..self.size.1),
            rng.gen_range(0..self.size.2),
        );
        while self.get(coord) {
            coord = (
                rng.gen_range(0..self.size.0),
                rng.gen_range(0..self.size.1),
                rng.gen_range(0..self.size.2),
            );
        }
        coord
    }

    pub fn find_random_direction(&self, pos: Coordinate) -> Direction {
        let mut rng = rand::thread_rng();
        let mut open_dirs: Vec<Direction> = Vec::new();

        for dir in Direction::iterator() {
            if self.count_available_in_direction(pos, *dir) > 0 {
                open_dirs.push(*dir);
            }
        }

        match open_dirs.len() {
            0 => panic!("No available directions found"),
            _ => return open_dirs[rng.gen_range(0..open_dirs.len())],
        }
    }
}

#[cfg(test)]
mod tests {
    use std::panic;

    #[test]
    fn node_struct_new_all_zero_input_panic() {
        let result = panic::catch_unwind(|| {
            super::NodeStruct::new((0, 0, 0));
        });
        assert!(result.is_err());
    }

    #[test]
    fn node_struct_new_any_zero_input_panic() {
        let result = panic::catch_unwind(|| {
            super::NodeStruct::new((1, 0, 1));
        });
        assert!(result.is_err());
    }

    #[test]
    fn node_struct_new_negative_input_panic() {
        let result = panic::catch_unwind(|| {
            super::NodeStruct::new((-1, 1, 1));
        });
        assert!(result.is_err());
    }

    #[test]
    fn node_struct_new_positive_input_no_panic() {
        let result = panic::catch_unwind(|| {
            super::NodeStruct::new((1, 1, 1));
        });
        assert!(result.is_ok());
    }

    #[test]
    fn node_struct_to_vec_index_out_of_bounds_panic() {
        let node_struct = super::NodeStruct::new((1, 1, 1));
        let result = panic::catch_unwind(|| {
            node_struct.to_vec_index((1, 1, 1));
        });
        assert!(result.is_err());
    }

    #[test]
    fn node_struct_to_vec_index_in_bounds_no_panic() {
        let node_struct = super::NodeStruct::new((1, 1, 1));
        let result = panic::catch_unwind(|| {
            node_struct.to_vec_index((0, 0, 0));
        });

        assert!(result.is_ok());
    }

    #[test]
    fn node_struct_to_vec_index_propertly_calculates_index() {
        let node_struct = super::NodeStruct::new((2, 2, 2));
        assert_eq!(node_struct.to_vec_index((0, 0, 0)), 0);
        assert_eq!(node_struct.to_vec_index((0, 0, 1)), 1);
        assert_eq!(node_struct.to_vec_index((0, 1, 0)), 2);
        assert_eq!(node_struct.to_vec_index((0, 1, 1)), 3);
        assert_eq!(node_struct.to_vec_index((1, 0, 0)), 4);
        assert_eq!(node_struct.to_vec_index((1, 0, 1)), 5);
        assert_eq!(node_struct.to_vec_index((1, 1, 0)), 6);
        assert_eq!(node_struct.to_vec_index((1, 1, 1)), 7);
    }

    // This is probably redundant, whatever
    #[test]
    fn node_struct_to_vec_index_larger_size_propertly_calculates_index() {
        let node_struct = super::NodeStruct::new((3, 3, 3));
        assert_eq!(node_struct.to_vec_index((0, 0, 0)), 0);
        assert_eq!(node_struct.to_vec_index((0, 0, 1)), 1);
        assert_eq!(node_struct.to_vec_index((0, 0, 2)), 2);
        assert_eq!(node_struct.to_vec_index((0, 1, 0)), 3);
        assert_eq!(node_struct.to_vec_index((0, 1, 1)), 4);
        assert_eq!(node_struct.to_vec_index((0, 1, 2)), 5);
        assert_eq!(node_struct.to_vec_index((0, 2, 0)), 6);
        assert_eq!(node_struct.to_vec_index((0, 2, 1)), 7);
        assert_eq!(node_struct.to_vec_index((0, 2, 2)), 8);
        assert_eq!(node_struct.to_vec_index((1, 0, 0)), 9);
        assert_eq!(node_struct.to_vec_index((1, 0, 1)), 10);
        assert_eq!(node_struct.to_vec_index((1, 0, 2)), 11);
        assert_eq!(node_struct.to_vec_index((1, 1, 0)), 12);
        assert_eq!(node_struct.to_vec_index((1, 1, 1)), 13);
        assert_eq!(node_struct.to_vec_index((1, 1, 2)), 14);
        assert_eq!(node_struct.to_vec_index((1, 2, 0)), 15);
        assert_eq!(node_struct.to_vec_index((1, 2, 1)), 16);
        assert_eq!(node_struct.to_vec_index((1, 2, 2)), 17);
        assert_eq!(node_struct.to_vec_index((2, 0, 0)), 18);
        assert_eq!(node_struct.to_vec_index((2, 0, 1)), 19);
        assert_eq!(node_struct.to_vec_index((2, 0, 2)), 20);
        assert_eq!(node_struct.to_vec_index((2, 1, 0)), 21);
        assert_eq!(node_struct.to_vec_index((2, 1, 1)), 22);
        assert_eq!(node_struct.to_vec_index((2, 1, 2)), 23);
        assert_eq!(node_struct.to_vec_index((2, 2, 0)), 24);
        assert_eq!(node_struct.to_vec_index((2, 2, 1)), 25);
        assert_eq!(node_struct.to_vec_index((2, 2, 2)), 26);
    }

    #[test]
    fn node_struct_get_out_of_bounds_panic() {
        let node_struct = super::NodeStruct::new((1, 1, 1));
        let result = panic::catch_unwind(|| {
            node_struct.get((1, 1, 1));
        });
        assert!(result.is_err());
    }

    #[test]
    fn node_struct_get_in_bounds_no_panic() {
        let node_struct = super::NodeStruct::new((1, 1, 1));
        let result = panic::catch_unwind(|| {
            node_struct.get((0, 0, 0));
        });
        assert!(result.is_ok());
    }

    #[test]
    #[should_panic]
    fn node_struct_set_out_of_bounds_panic() {
        let mut node_struct = super::NodeStruct::new((1, 1, 1));
        node_struct.set((1, 1, 1), true);
    }

    #[test]
    fn node_struct_set_in_bounds_no_panic() {
        let mut node_struct = super::NodeStruct::new((1, 1, 1));
        node_struct.set((0, 0, 0), true);
    }

    #[test]
    fn node_struct_set_in_bounds_get_same_value() {
        let mut node_struct = super::NodeStruct::new((1, 1, 1));
        node_struct.set((0, 0, 0), true);
        assert_eq!(node_struct.get((0, 0, 0)), true);
    }

    #[test]
    fn node_struct_larger_size_set_in_bounds_get_same_value() {
        let mut node_struct = super::NodeStruct::new((2, 2, 2));
        node_struct.set((1, 1, 1), true);
        assert_eq!(node_struct.get((1, 1, 1)), true);
    }

    #[test]
    fn node_struct_larger_size_set_multiple_in_bounds_get_same_value() {
        let mut node_struct = super::NodeStruct::new((4, 4, 4));
        node_struct.set((2, 2, 2), true);
        node_struct.set((1, 1, 0), true);
        assert_eq!(node_struct.get((2, 2, 2)), true);
        assert_eq!(node_struct.get((1, 1, 0)), true);
    }
}
