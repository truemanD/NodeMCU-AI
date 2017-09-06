SCHEDULER.every '1m', :first_in => 0 do |job|
  send_event('BercutAnaliticsCountTasks', { current: rand(100), last: rand(100) })
end