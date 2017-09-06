SCHEDULER.every '1m', :first_in => 0 do |job|
  send_event( 'release33', {title: "InVoice 4.7.33.0", progress_items: [
        {name: "211715", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "210124", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "212316", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "212845", progress: 100, critical: 80, warning: 30, localScope: 0}]} )
    
  send_event( 'release34', {title: "InVoice 4.7.34.0", progress_items: [
        {name: "208811", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "210098", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "210079", progress: 100,critical: 80, warning: 30, localScope: 0},
        {name: "210526", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "209075", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "208733", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "207758", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "210124", progress: 100, critical: 80, warning: 30, localScope: 0}]})

  send_event( 'release35', {title: "InVoice 4.7.35.0", progress_items: [
        {name: "212501", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "211887", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "210053", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "207121", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "210911", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "210737", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "211850", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "211118", progress: rand(100), critical: 80, warning: 30, localScope: 0}]})

  send_event( 'release36', {title: "InVoice 4.7.36.0", progress_items: [
        {name: "210935", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "211789", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "209483", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "209661", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "212428", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "205336", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "212914", progress: rand(100), critical: 80, warning: 30, localScope: 0},
        {name: "213193", progress: rand(100), critical: 80, warning: 30, localScope: 0}]})

  send_event( 'release37', {title: "InVoice 4.7.37.0", progress_items: [
        {name: "210460", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "204366", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "204364", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "211444", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "212817", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "210491", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "210156", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "209714", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "209546", progress: rand(10), critical: 80, warning: 30, localScope: 0},
        {name: "204358", progress: rand(10), critical: 80, warning: 30, localScope: 0}]})
  
  send_event( 'release38', {title: "InVoice 4.7.38.0", progress_items: [
        {name: "210460", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "204366", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "204364", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "211444", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "212817", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "210491", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "210156", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "209714", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "209546", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "204358", progress: 0, critical: 80, warning: 30, localScope: 0}]})
  
   send_event( 'release39', {title: "InVoice 4.7.39.0", progress_items: [
        {name: "210460", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "204366", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "204364", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "211444", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "212817", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "210491", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "210156", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "209714", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "209546", progress: 0, critical: 80, warning: 30, localScope: 0},
        {name: "204358", progress: 0, critical: 80, warning: 30, localScope: 0}]})
  
  send_event( 'releaseWSMS', {title: "Welcome SMS", progress_items: [
        {name: "210460", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "204366", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "204364", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "211444", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "212817", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "210491", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "210156", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "209714", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "209546", progress: 100, critical: 80, warning: 30, localScope: 0},
        {name: "204358", progress: 100, critical: 80, warning: 30, localScope: 0}]})
end