config_file = File.dirname(File.expand_path(__FILE__)) + '/../conf/agile_timeline_data.yml'

SCHEDULER.every '10s', :first_in => 0 do |job|
  config = YAML::load(File.open(config_file))
  unless config["events"].nil?
    events =  Array.new
    config["events"].each do |event|
      events << {
        label: event["name"],
        value: event["problem"]
      }
    end

  end
  send_event('agile_list', { unordered:true, items: events })
end
