use clap::ArgMatches;

use rust_pipes::Color;

pub(crate) struct DrawOptions {
    // pub original_colors: bool,
    pub bg_color: Color,
    pub angle_subdiv: u32,
}

impl DrawOptions {
    pub fn new(cli_match: &ArgMatches) -> DrawOptions {
        let bg_color_vec: Vec<u8> = cli_match
            .get_many("bg-color")
            .expect("This field should have a value populated!")
            .copied()
            .collect();

        let bg_tuple: Color = (bg_color_vec[0], bg_color_vec[1], bg_color_vec[2]);

        DrawOptions {
            bg_color: bg_tuple,
            angle_subdiv: *cli_match
                .get_one("angle-subdiv")
                .expect("Arg angle-subdiv should be populated"),
            // original_colors: *cli_match
            //     .get_one("original-colors")
            //     .expect("Arg original-colors should be populated"),
        }
    }
}

pub(crate) struct WorldOptions {
    pub max_pipes: u8,
    pub max_bounds: (u8, u8, u8),
    pub turn_chance: f32,
    pub max_gen_time: u32,
    pub max_freeze_time: u32,
}

impl WorldOptions {
    pub fn new(cli_match: &ArgMatches) -> WorldOptions {
        let bounds_vec: Vec<u8> = cli_match
            .get_many("max-bounds")
            .expect("This field should have a value populated!")
            .copied()
            .collect();

        let bounds_tuple: (u8, u8, u8) = (bounds_vec[0], bounds_vec[1], bounds_vec[2]);

        WorldOptions {
            max_pipes: *cli_match
                .get_one("max-pipes")
                .expect("Arg max-pipes should be populated"),
            max_bounds: bounds_tuple,
            turn_chance: *cli_match
                .get_one("turn-chance")
                .expect("Arg turn-chance should be populated"),
            max_gen_time: *cli_match
                .get_one("max-gen-time")
                .expect("Arg max-gen-time should be populated"),
            max_freeze_time: *cli_match
                .get_one("max-freeze-time")
                .expect("Arg max-freeze-time should be populated"),
        }
    }
}

///Internal struct to organize configuration and pass it around to necessary parts of the program
///
pub(crate) struct Configuration {
    pub draw: DrawOptions,
    pub world: WorldOptions,
}

impl Configuration {
    pub fn new(cli_match: &ArgMatches) -> Configuration {
        Configuration {
            draw: DrawOptions::new(cli_match),
            world: WorldOptions::new(cli_match),
        }
    }
}
