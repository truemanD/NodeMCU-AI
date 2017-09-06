config_file = File.dirname(File.expand_path(__FILE__)) + '/../conf/key_projects_data.yml'

SCHEDULER.every '10s', :first_in => 0 do |job|
  config = YAML::load(File.open(config_file))
  unless config["events"].nil?
    events =  Array.new
    config["events"].each do |event|
      events << {
        name: event["name"],
        label: event["name"],
        progress: event["progress"],
        critical: 80,
        warning: 30,
        localScope: 0,
        value: event["problem"]
      }
    end

  end
  
#    send_event( 'key_projects', {title: "Ключевые проекты", progress_items: events} )
    send_event( 'key_projects', { progress_items: events} )
    send_event('key_projects_problems', { unordered:true, items: events })

end