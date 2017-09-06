labels = ['< 5 день', '~ 5 дней', '~ 1 месяц','> 1 месяц']

SCHEDULER.every '2m', :first_in => 0 do |job|

  data = [
    {
      # Create a random set of data for the chart
      label: 'Количество оцененных запросов',
      data: Array.new(5) { rand(30) },
      backgroundColor: [
        '#00cc66',
        '#00cc66',
        '#00cc66',
        '#00cc66'
      ],
      hoverBackgroundColor: [
        '#FF6384',
        '#FF6384',
        '#FF6384',
        '#FF6384'
 
      ],
    },
  ]

  send_event('preparedTasks', { labels: labels, datasets: data })
end