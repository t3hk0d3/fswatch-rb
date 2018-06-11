require 'spec_helper'

RSpec.describe Fswatch::Monitor do
  recorded_events = []

  options = {
    callback: -> (events) { recorded_events += events },
    path: './tmp/',
    properties: {
      foo: "bar"
    },
    event_flags: [:created, :updated, :is_file, :renamed, :removed],
    filters: {
      /\.ex$/ix => :exclude, # if file ends with \.ex - it should be excluded
      /\.in$/ix => :include  # if file ends with \.in - it should be included
    },
    latency: 0.1, # 100ms
    allow_overflow: true,
    recursive: true,
    follow_symlinks: true,
    directory_only: true,
    # debug: true, # use only for debugging
  }

  before(:context) do
    @monitor = described_class.new(options)

    @thread = Thread.new { @monitor.start }

    Thread.pass while !@monitor.running?
  end

  after(:context) do
    @monitor.stop
    recorded_events = []
  end

  context 'monitor filesystem changes' do
    before(:context) do
      excluded_file = "tmp/testfile.ex"

      begin 
        File.open('tmp/testfile.in', 'wb') { |f| f.write('test') }
        File.open('tmp/testfile.ex', 'wb') { |f| f.write('test') }
      ensure 
        File.unlink('tmp/testfile.ex')
        File.unlink('tmp/testfile.in')
      end

      Thread.pass while recorded_events.empty?
    end

  	it 'capture filesystem changes' do
      expect(recorded_events).to include(
        [ match(%r{tmp/testfile.in\Z}), be_within(2).of(Time.now.to_i), :created, :removed, :updated, :is_file ]
      )
  	end

    it 'do not capture files excluded by filter' do
      expect(recorded_events).not_to include(
        start_with(match(%r{tmp/testfile.ex\Z}))
      )
    end
  end
end

