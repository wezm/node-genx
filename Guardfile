# A sample Guardfile
# More info at https://github.com/guard/guard#readme

# Add files and commands to this file, like the example:
#   watch('file/path') { `command(s)` }
#
guard 'shell' do
  watch(%r".*\.(?:cc|h)") { |m| `node-waf build && jasbin` }
  watch(%r"spec/.*_spec\..*") {|m| `jasbin --verbose` }
end
