list = ['Согласование требований','Реализация','Отгрузка','Приемка','Запуск'] 
word_counts = Hash.new({ value: 0 })

SCHEDULER.every '2s' do
  random_word = list.sample
  word_counts[random_word] = { label: random_word, value: (word_counts[random_word][:value] + 1) % 30 }
  
  send_event('list', { unordered:true, items: word_counts.values })
end