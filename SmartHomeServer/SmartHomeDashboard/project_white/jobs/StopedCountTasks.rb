SCHEDULER.every '6m', :first_in => 0 do |job|
  send_event('StopedCountTasks', { current: rand(100), last: rand(100) })
end