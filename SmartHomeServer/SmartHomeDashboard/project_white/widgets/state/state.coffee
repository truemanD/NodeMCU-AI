class Dashing.State extends Dashing.Widget
  @accessor 'FCurrentColor', Dashing.AnimatedValue

  @accessor 'FCurrentState', Dashing.AnimatedValue

  @accessor 'FPreviousState', Dashing.AnimatedValue

  ready: ->

    # This is fired when the widget is done being rendered      

    $(@node).css('background-color',@get('FCurrentColor'))


  onData: (data) ->
    if data.status
      # clear existing "status-*" classes
      $(@get('node')).attr 'class', (i,c) ->
        c.replace /\bstatus-\S+/g, ''
      # add new class
      $(@get('node')).addClass "status-#{data.status}"



    if data.FCurrentState isnt data.FPreviousState
      $(@node).fadeOut().css('background-color',@get('FCurrentColor')).fadeIn()