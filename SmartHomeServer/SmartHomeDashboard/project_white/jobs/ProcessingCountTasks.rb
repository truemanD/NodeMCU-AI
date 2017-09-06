SCHEDULER.every '4m', :first_in => 0 do |job|
  send_event('ProcessingCountTasks', { current: rand(100), last: rand(100) })
end