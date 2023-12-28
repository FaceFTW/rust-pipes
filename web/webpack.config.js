
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const webpack = require('webpack');
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
		// Have this example work in Edge which doesn't ship `TextEncoder` or
		// `TextDecoder` at this time.
		new webpack.ProvidePlugin({
			TextDecoder: ['text-encoding', 'TextDecoder'],
			TextEncoder: ['text-encoding', 'TextEncoder']
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
