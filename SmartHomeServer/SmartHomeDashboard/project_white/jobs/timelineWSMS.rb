MAX_DAYS_OVERDUE = -2000
MAX_DAYS_AWAY = 900

#35
config_file = File.dirname(File.expand_path(__FILE__)) + '/../conf/timeline_dataWSMS.yml'

SCHEDULER.every '1m', :first_in => 0 do |job|
  config = YAML::load(File.open(config_file))
  unless config["events"].nil?
    events =  Array.new
    today = Date.today
    no_event_today = true
    config["events"].each do |event|
      days_away = (Date.parse(event["date"]) - today).to_i
      if (days_away >= 0) && (days_away <= MAX_DAYS_AWAY) 
        events << {
          name: event["name"],
          date: event["date"],
          background: event["background"],
          url: event["url"]
        }
      elsif (days_away < 0) && (days_away >= MAX_DAYS_OVERDUE)
        events << {
          name: event["name"],
          date: event["date"],
          background: event["background"],
          url: event["url"],
          opacity: 0.5
        }
      end

      no_event_today = false if days_away == 0
    end

    if no_event_today
      events << {
        name: "СЕГОДНЯ",
        date: today.strftime('%a %d %b %Y'),
        background: "blue"
      }
    end

    send_event("timelineWSMS", {events: events})
  else
    puts "No events found :("
  end
end