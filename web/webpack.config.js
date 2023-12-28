
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CopyPlugin = require('copy-webpack-plugin');

module.exports = {
	entry: './index.js',
	output: {
		path: path.resolve(__dirname, 'dist'),
		filename: 'index.js',
		clean: true
	},
	plugins: [
		new HtmlWebpackPlugin({
			template: 'index.html'
		}),
		new CopyPlugin({
			patterns: [{
				from: "public",
				to: "."
			}]
		})
	],

	experiments: {
		asyncWebAssembly: true
	}
};
