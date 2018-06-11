module Fswatch
  class Watcher

    def initialize(options = {})
      @event_handlers = []

      options[:callback] = self.method(:_event_handler)

      @monitor = Fswatch::Monitor.new(options)
    end

    def watch(options = {}, &block)
      raise 'Invalid handler' unless block

      options = nil if options.empty? # reduce memory usage

      @event_handlers << [block, options]

      return nil
    end

    def start!
      raise 'Already running!' if running?

      @thread = Thread.new { @monitor.start }
    end

    def stop!
      raise 'Not running!' if running?

      @monitor.stop
    end

    def running?
      @monitor.running?
    end

    private

    def _event_handler(events)
      events.each do |event|
        filename, timestamp, *event_flags = event

        @event_handlers.each do |handler, options|
          if options
            # match filename
            next if options[:match] && !options[:match].match?(filename)

            # event
            next if options[:on] && (options[:on] & event_flags).empty?
          end

          handler.call(filename, timestamp, event_flags, options)
        end
      end
    end

  end
end
