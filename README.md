# fswatch-rb

fswatch-rb is binding for multi-platform fswatch library.

https://github.com/emcrisostomo/fswatch/

## Installation

First you need to install fswatch >= 1.11.3

```bash
$ brew install fswatch
```

Then add this line to your application's Gemfile:

```ruby
gem 'fswatch-rb'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install fswatch-rb

## Usage

```ruby
require 'fswatch'

events_array = []

watcher = Fswatch::Watcher.new(
  path: '~/my/awesome/project/directory/',
  event_flags: [:created, :updated, :is_file, :renamed, :removed],
  filters: { # global filters, high-perfomance
    /\.tmp$/ix => :exclude, # if file ends with .tmp it will be ignored
    /\(.rb|.slim)$/x => :include  # include only .rb and .slim files
    # Please note these reg exps are executed OUTSIDE ruby and not by ruby regexp implementation, so be careful
  },
  latency: 0.1, # 100ms
  recursive: true,
  follow_symlinks: true,
)

watcher.watch do |file, timestamp, flags|
  puts "File #{file} has been changed at #{timestamp} with #{flags}"
end

# local filename filter, low-perfomance
watcher.watch(match: /\.css$/) do |file, timestamp, flags|
  puts "CSS File #{file} has been changed!"
end

watcher.start! # this will spawn new thread in background

watcher.running? # => true

# ... do evil stuff ...

watcher.stop!

watcher.running? # => false

```

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/t3hk0d3/fswatch-rb.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
