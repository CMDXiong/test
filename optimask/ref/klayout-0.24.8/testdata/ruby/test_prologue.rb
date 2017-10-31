
Object.const_defined?(:TestBase) && Object.send(:remove_const, :TestBase)

begin 

  # try to load test/unit
  require 'test/unit/ui/console/testrunner'

  TestBase = Test::Unit::TestCase

  $__use_minitest = false
  puts "INFO: using test/unit ..."

rescue

  require "minitest/unit"

  # 5.0.0 does not have run method
  if MiniTest::Unit::VERSION >= "5.0.0"
    raise "Minitest version <5.0.0 required"
  end

  TestBase = MiniTest::Unit::TestCase

  puts "INFO: using minitest/unit ..."
  $__use_minitest = true

end 

# in MSVC environment:
if ENV["RUBY"] 
  $:.push("#{ENV["RUBY"]}/lib/ruby/#{RUBY_VERSION}")
end

# Set this to true to disable some tests involving exceptions
$leak_check = ENV["TEST_LEAK_CHECK"]

# undefine existing classes

Object.constants.each do |c|
  if c.to_s =~ /_TestClass$/
    Object.send(:remove_const, c)
  end
end

