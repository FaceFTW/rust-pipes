#[cfg(not(target_arch = "wasm32"))]
use clap::ArgMatches;

use crate::engine::util::Color;

pub(crate) struct DrawOptions {
    pub bg_color: Color,
    pub angle_subdiv: u32,
}

impl DrawOptions {
    #[cfg(not(target_arch = "wasm32"))]
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
    #[cfg(not(target_arch = "wasm32"))]
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

///Internal struct to organize configuration and
/// pass it around to necessary parts of the program
///
pub(crate) struct Configuration {
    pub draw: DrawOptions,
    pub world: WorldOptions,
    pub single_run: bool,
    pub rng_seed: Option<u64>,
    pub window_size: Option<(u32, u32)>,
}

impl Configuration {
    #[cfg(not(target_arch = "wasm32"))]
    pub fn new(cli_match: &ArgMatches) -> Configuration {
        Configuration {
            draw: DrawOptions::new(cli_match),
            world: WorldOptions::new(cli_match),
            single_run: *cli_match
                .get_one("single-run")
                .expect("Arg single-run should be populated"),
            rng_seed: match cli_match.get_one("rng-seed").unwrap_or(&None) {
                Some(val) => Some(*val),
                None => None,
            },
            window_size: match cli_match.get_many("window-size") {
                Some(vals) => {
                    let vector: Vec<u32> = vals.copied().collect();
                    Some((vector[0], vector[1]))
                }
                None => None,
            },
        }
    }

    #[cfg(target_arch = "wasm32")]
    pub fn new() -> Configuration {
        Configuration {
            draw: DrawOptions {
                bg_color: (40, 40, 40),
                angle_subdiv: 16,
            },
            world: WorldOptions {
                max_pipes: 8,
                max_bounds: (20, 20, 20),
                turn_chance: 0.3,
                max_gen_time: 15,
                max_freeze_time: 10,
            },
            single_run: false,
            rng_seed: None,
            window_size: None,
        }
    }
}

///Takes a string and effectively converts it to a u64 seed
/// that is usable by the fastrand library. Primarily used as
/// a clap value parser. Should not return errors in any situation,
/// Result<T,E> is for type checking.
///
/// Fun fact, I am basing this roughly off of how Minecraft takes level
/// generation seeds but downcasting to u64 size instead of 128-bit
/// (See net.minecraft.world.level.levelgen.RandomSupport)
#[cfg(not(target_arch = "wasm32"))]
fn parse_seed(val: &str) -> Result<Option<u64>, std::io::Error> {
    match val.parse::<u64>() {
        Ok(val) => Ok(Some(val)),
        Err(_) => {
            let hash: [u8; 16] = md5::compute(val).into();
            //Do some funny bit ops to make the full 64-bit number
            //We intentionally discard half of the hash because no shot
            //128 bits can fit into 64 bits

            let seed_u64: u64 = ((hash[0] & 0xFF) as u64) << 56
                | ((hash[1] & 0xFF) as u64) << 48
                | ((hash[2] & 0xFF) as u64) << 40
                | ((hash[3] & 0xFF) as u64) << 32
                | ((hash[4] & 0xFF) as u64) << 24
                | ((hash[5] & 0xFF) as u64) << 16
                | ((hash[6] & 0xFF) as u64) << 8
                | ((hash[7] & 0xFF) as u64);
            Ok(Some(seed_u64))
        }
    }
}

#[cfg(not(target_arch = "wasm32"))]
use clap::{command, Arg, ArgAction, ArgGroup, Command};

#[cfg(not(target_arch = "wasm32"))]
pub fn make_cli_parser() -> Command {
    use clap::builder::ValueParser;

    command!()
        .group(ArgGroup::new("exec").required(false).multiple(true))
        .next_help_heading("Execution Options")
        .args([
            Arg::new("rng-seed")
                .long("seed")
                .value_name("seed")
                .help("The seed input used by the random number generator for pipe generation.\n The state of the random number generator is not reset between simulations.\n Seeds can be alphanumeric.",)
                .value_parser(ValueParser::new(parse_seed)),
            Arg::new("window-size")
                .long("win-size")
                .num_args(2)
                .value_parser(clap::value_parser!(u32).range(1..))
                .value_names(["width", "height"])
                .default_values(["1280","720"])
        ])
        .group(ArgGroup::new("sim").required(false).multiple(true))
        .next_help_heading("Simulation Options")
        .args([
            Arg::new("single-run")
                .long("single-run")
                .default_value("false")
                .action(ArgAction::SetTrue)
                .help("If the simulation should be run only once"),
            //================ --max-pipes ================
            Arg::new("max-pipes")
                .short('p')
                .long("max-pipes")
                .value_parser(clap::value_parser!(u8).range(1..))
                .default_value("8")
                .help("Maximum number of pipes to create in each simulation run"),
            //================ --max-bounds ================
            Arg::new("max-bounds")
                .long("max-bounds")
                .num_args(3)
                .value_parser(clap::value_parser!(u8).range(8..))
                .value_names(["x", "y", "z"])
                .default_values(["20", "20", "20"]),
            //================ --turn-chance ================
            Arg::new("turn-chance")
                .long("turn-chance")
                .value_parser(clap::value_parser!(f32))
                .default_value("0.3"),
            //================ --max-gen-time ================
            Arg::new("max-gen-time")
                .long("max-gen-time")
                .value_parser(clap::value_parser!(u32))
                .default_value("15"),
            //================ --max-freeze-time ================
            Arg::new("max-freeze-time")
                .long("max-freeze-time")
                .value_parser(clap::value_parser!(u32))
                .default_value("10")
                .conflicts_with("single-run"),
        ])
        .group(ArgGroup::new("render").required(false).multiple(true))
        .next_help_heading("Rendering Options")
        .args([
            Arg::new("angle-subdiv")
                .value_parser(clap::value_parser!(u32).range(8..))
                .default_value("16")
                .long("angle-subdiv"),
            Arg::new("original-colors")
                .long("original-colors")
                .default_value("false")
                .value_name("true/false")
                .value_parser(clap::value_parser!(bool))
                .help("If we should only render using the colors defined in the original screensaver."),
            Arg::new("bg-color")
                .long("bg-color")
                .num_args(3)
                .value_names(["red","green","blue"])
                .default_values(["10","10","10"])
                .value_parser(clap::value_parser!(u8))
        ])
}

//Test cfg should imply not(target_arch="wasm32")
#[cfg(test)]
mod tests {
    use super::{make_cli_parser, parse_seed};

    #[test]
    pub fn cli_single_run_flag_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--single-run"]);
        assert!(match1.contains_id("single-run"));
        assert!(match1.get_flag("single-run"));

        //recreate because of rust semantics (also good for ensuring clean state)
        let cli_parser = make_cli_parser();
        let match2 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match2.contains_id("single-run"));
        assert!(!match2.get_flag("single-run"));
    }

    #[test]
    pub fn cli_max_pipe_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-pipes"));
        let val1: u8 = *match1
            .get_one("max-pipes")
            .expect("Default value should have been configured");
        assert_eq!(val1, 8);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-pipes", "100"]);
        assert!(match1.contains_id("max-pipes"));
        let val1: u8 = *match1
            .get_one::<u8>("max-pipes")
            .expect("Default value should have been configured");
        assert_eq!(val1, 100);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "-p", "50"]);
        assert!(match1.contains_id("max-pipes"));
        let val1: u8 = *match1
            .get_one("max-pipes")
            .expect("Default value should have been configured");
        assert_eq!(val1, 50);
    }

    #[test]
    pub fn cli_max_bound_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-bounds"));
        let val1 = match1
            .get_many::<u8>("max-bounds")
            .expect("Default value should have been configured")
            .copied()
            .collect::<Vec<u8>>();
        assert_eq!(val1, vec![20, 20, 20]);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-bounds", "10", "11", "12"]);
        assert!(match1.contains_id("max-bounds"));
        let val1 = match1
            .get_many::<u8>("max-bounds")
            .expect("Default value should have been configured")
            .copied()
            .collect::<Vec<u8>>();
        assert_eq!(val1, vec![10, 11, 12]);
    }

    #[test]
    pub fn cli_max_gen_time_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-gen-time"));
        let val1: u32 = *match1
            .get_one("max-gen-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 15);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-gen-time", "100"]);
        assert!(match1.contains_id("max-gen-time"));
        let val1: u32 = *match1
            .get_one::<u32>("max-gen-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 100);
    }

    #[test]
    pub fn cli_max_freeze_time_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-freeze-time"));
        let val1: u32 = *match1
            .get_one("max-freeze-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 10);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-freeze-time", "100"]);
        assert!(match1.contains_id("max-freeze-time"));
        let val1: u32 = *match1
            .get_one::<u32>("max-freeze-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 100);
    }

    #[test]
    pub fn cli_turn_chance_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("turn-chance"));
        let val1: f32 = *match1
            .get_one("turn-chance")
            .expect("Default value should have been configured");
        assert_eq!(val1, 0.3);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--turn-chance", "0.75"]);
        assert!(match1.contains_id("turn-chance"));
        let val1: f32 = *match1
            .get_one::<f32>("turn-chance")
            .expect("Default value should have been configured");
        assert_eq!(val1, 0.75);
    }

    #[test]
    pub fn parse_seed_parsable_u64_returned() {
        let result = parse_seed("432");
        assert_eq!(result.unwrap(), Some(432));
    }
    #[test]
    pub fn parse_seed_parsed_string_u64_returned() {
        let result = parse_seed("lmao");
        assert_eq!(result.unwrap(), Some(1087897816963742493));
    }
}
