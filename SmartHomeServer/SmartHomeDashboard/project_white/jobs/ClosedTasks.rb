labels = ['< 1 месяц', '~ 2 месяц', '~ 3 месяц','~ 6 месяц','> 6 месяц']

SCHEDULER.every '1m', :first_in => 0 do |job|

  data = [
    {
      # Create a random set of data for the chart
      label: 'Количество завершенных запросов',
      data: Array.new(5) { rand(30) },
      backgroundColor: [
        '#00cc66',
        '#00cc66',
        '#00cc66',
        '#00cc66',
        '#00cc66',
      ],
      hoverBackgroundColor: [
        '#FF6384',
        '#FF6384',
        '#FF6384',
        '#FF6384',
        '#FF6384',
      ],
    },
  ]

  send_event('closedTasks', { labels: labels, datasets: data })
end