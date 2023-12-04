use clap::ArgMatches;

use crate::util::Color;

pub(crate) struct DrawOptions {
    pub original_colors: bool,
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
            original_colors: *cli_match
                .get_one("original-colors")
                .expect("Arg original-colors should be populated"),
        }
    }
}

pub(crate) struct WorldOptions {}

impl WorldOptions {
    pub fn new(cli_match: &ArgMatches) -> WorldOptions {
        WorldOptions {}
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
