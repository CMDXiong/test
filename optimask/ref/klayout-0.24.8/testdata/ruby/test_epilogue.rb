
# In the test environment, we cannot make sure that we destroy the ruby interpreter before the RBA
# environment is shut down. Therefore we must release all RBA objects by explicitly calling the GC
# and start the test suite manually.

if !$__use_minitest

  err = 0
  any = nil

  Object.constants.each do |c|
    if c.to_s =~ /_TestClass$/
      r = Test::Unit::UI::Console::TestRunner.run(Object.const_get(c))    
      err += r.error_count + r.failure_count
      any = true
    end
  end

  if !any
    raise("No test class defined (any ending with _TestClass)")
  end

  if err > 0
    raise("Tests failed (#{err} Errors + Failures)")
  end

else

  # run with minitest/unit:
  # Make sure that _test_FIRST is run as the first one
  err = MiniTest::Unit.new.run(["-n", "/^test_FIRST$/"])
  err ||= MiniTest::Unit.new.run(["-n", "/^test_\\d/"])

  GC.start

  if err > 0
    raise("Tests failed (#{err} Errors + Failures)")
  end

end

