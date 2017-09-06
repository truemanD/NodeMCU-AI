SCHEDULER.every '2m', :first_in => 0 do |job|
  send_event('CommonCountTasks', { current: rand(1000), last: rand(1000) })
end