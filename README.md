# LZ77::Konami

konami's lz77

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'lz77-konami'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install lz77-konami

## Usage

```ruby
encoded = LZ77::Konami.encode("howdy!" + ":) " * 10)
# => "\xFFhowdy!:)\x01 \x000\x00c\x00\xC9\x01#\x00\x00\x00"
LZ77::Konami.decode(encoded)
# => "howdy!:) :) :) :) :) :) :) :) :) :) "
```

## Development

After checking out the repo, run `bin/setup` to install dependencies. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and tags, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://gitlab.com/naari3/lz77-konami.
