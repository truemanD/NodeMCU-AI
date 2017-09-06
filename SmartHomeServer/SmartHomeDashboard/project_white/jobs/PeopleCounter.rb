SCHEDULER.every '3m', :first_in => 0 do |job|
  send_event('PeopleCounter', { current: rand(100), last: rand(100) })
end