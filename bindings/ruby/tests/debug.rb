# client.rb

require 'test/unit'
require './rbwsman'

class WsmanTest < Test::Unit::TestCase
  def test_debug
    Rbwsman::debug = 5
    assert Rbwsman::debug == 5
  end
end

