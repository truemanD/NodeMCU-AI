
SCHEDULER.every '3m', :first_in => 0 do |job|
  project1 = rand(100)

  if project1 > 30 && project1 < 60
    status = 'danger'
  end
  if project1 >= 60
    status = 'ok'
  end
  if project1 <= 30
    status = 'warning'
  end
  send_event('text_project_1', { value: project1, status: status})
end

SCHEDULER.every '30s', :first_in => 0 do |job|
  project2 = rand(100)

  if project2 > 30 && project2 < 60
    status = 'danger'
  end
  if project2 >= 60
    status = 'ok'
  end
  if project2 <= 30
    status = 'warning'
  end
  send_event('text_project_2', { value: project2, status: status})
end

SCHEDULER.every '1m', :first_in => 0 do |job|
  project3 = rand(100)

  if project3 > 30 && project3 < 60
    status = 'danger'
  end
  if project3 >= 60
    status = 'ok'
  end
  if project3 <= 30
    status = 'warning'
  end
  send_event('text_project_3', { value: project3, status: status})
end

SCHEDULER.every '1m', :first_in => 0 do |job|
  val = rand(100)
  if val <=50
    val = 50
  end
  if val > 30 && val < 60
    status = 'danger'
  end
  if val >= 60
    status = 'ok'
  end
  if val <= 30
    status = 'warning'
  end
  send_event('meter_done', { value: val, status: status})
end

SCHEDULER.every '2m', :first_in => 0 do |job|
  val = rand(110)
  if val <=90
    val = 90
  end
  if val > 30 && val < 60
    status = 'danger'
  end
  if val >= 60 
    status = 'ok'
  end
  if val <= 30
    status = 'warning'
  end
  send_event('meter_acas', { value: val, status: status})
end

SCHEDULER.every '1m', :first_in => 0 do |job|
  val = rand(100)

  if val > 30 && val < 60
    status = 'danger'
  end
  if val >= 60
    status = 'warning'
  end
  if val <= 30
    status = 'ok'
  end
  send_event('meter', { value: val, status: status})
end

