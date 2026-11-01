extends Control
# LAST HAVEN: AFTER DARK — vertical slice 0.2
# Standalone Godot 4.x project. All visuals generated at runtime.
const SAVE_PATH := "user://last_haven_save.json"
const ROOM_DATA := {
 "Velitelství": {"cost": 0, "color": "#586b8b", "symbol": "⌂"},
 "Generátor": {"cost": 40, "color": "#d58b32", "symbol": "⚡"},
 "Farma": {"cost": 30, "color": "#4c9563", "symbol": "✿"},
 "Ložnice": {"cost": 25, "color": "#7776aa", "symbol": "▣"},
 "Ošetřovna": {"cost": 35, "color": "#4c9da0", "symbol": "+"},
 "Dílna": {"cost": 45, "color": "#a76c50", "symbol": "⚒"},
 "Obrana": {"cost": 55, "color": "#ad4c4c", "symbol": "⛨"}
}
var food := 65
var power := 80
var scrap := 125
var people := 8
var defense := 30
var morale := 75
var day := 1
var rooms: Array[String] = ["Velitelství", "Generátor", "Ložnice"]
var log_message := "Nouzový signál zachycen. Bunkr čeká na rozkazy."
var header: Label
var event_label: Label
var bunker_grid: GridContainer
var bunker_panel: Control
var build_buttons: Array[Button] = []
var surface: SubViewportContainer
var world_camera: Camera3D
var world_actor: Node3D
var in_exploration := false
var move_direction := Vector2.ZERO
var enemy_nodes: Array[Node3D] = []
var player_health := 100
var enemies_defeated := 0
var overlay: VBoxContainer
var explore_hud: VBoxContainer

func _ready() -> void:
 _make_interface()
 _refresh()

func _make_interface() -> void:
 var backdrop := ColorRect.new()
 backdrop.color = Color("#090e19")
 backdrop.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
 add_child(backdrop)
 overlay = VBoxContainer.new()
 overlay.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
 overlay.add_theme_constant_override("separation", 8)
 add_child(overlay)
 var top := PanelContainer.new()
 overlay.add_child(top)
 var topbox := VBoxContainer.new()
 top.add_child(topbox)
 var title := Label.new()
 title.text = "LAST HAVEN  /  AFTER DARK"
 title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
 title.add_theme_font_size_override("font_size", 27)
 title.add_theme_color_override("font_color", Color("#ffb65b"))
 topbox.add_child(title)
 header = Label.new()
 header.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
 header.add_theme_font_size_override("font_size", 17)
 topbox.add_child(header)
 event_label = Label.new()
 event_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
 event_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
 event_label.add_theme_color_override("font_color", Color("#ffcc8e"))
 topbox.add_child(event_label)
 var scroll := ScrollContainer.new()
 scroll.size_flags_vertical = Control.SIZE_EXPAND_FILL
 overlay.add_child(scroll)
 bunker_panel = VBoxContainer.new()
 bunker_panel.size_flags_horizontal = Control.SIZE_EXPAND_FILL
 scroll.add_child(bunker_panel)
 var ground := Label.new()
 ground.text = "☣ POVRCH — KARANTÉNNÍ ZÓNA ☣"
 ground.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
 ground.add_theme_color_override("font_color", Color("#e16b63"))
 bunker_panel.add_child(ground)
 bunker_grid = GridContainer.new()
 bunker_grid.columns = 3
 bunker_grid.size_flags_horizontal = Control.SIZE_EXPAND_FILL
 bunker_panel.add_child(bunker_grid)
 var build_title := Label.new()
 build_title.text = "VÝSTAVBA ZÁKLADNY"
 build_title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
 bunker_panel.add_child(build_title)
 var build_row := HFlowContainer.new()
 bunker_panel.add_child(build_row)
 for kind in ROOM_DATA.keys():
  if kind == "Velitelství":
   continue
  var button := Button.new()
  button.text = kind + "  " + str(ROOM_DATA[kind]["cost"]) + "⚙"
  button.custom_minimum_size = Vector2(150, 48)
  button.pressed.connect(_build.bind(kind))
  build_row.add_child(button)
  build_buttons.append(button)
 var actions := HFlowContainer.new()
 overlay.add_child(actions)
 _action(actions, "▶ DALŠÍ DEN", _next_day)
 _action(actions, "☣ VÝPRAVA 3D", _enter_exploration)
 _action(actions, "💾 ULOŽIT", _save)
 _action(actions, "↺ NAČÍST", _load)
 surface = SubViewportContainer.new()
 surface.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
 surface.visible = false
 surface.stretch = true
 add_child(surface)
 var viewport := SubViewport.new()
 viewport.size = Vector2i(1280, 720)
 viewport.render_target_update_mode = SubViewport.UPDATE_ALWAYS
 viewport.own_world_3d = true
 surface.add_child(viewport)
 _make_world(viewport)
 explore_hud = VBoxContainer.new()
 explore_hud.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
 explore_hud.visible = false
 explore_hud.mouse_filter = Control.MOUSE_FILTER_PASS
 add_child(explore_hud)
 var exploration_title := Label.new()
 exploration_title.text = "☣ POVRCH — PRŮZKUM  |  SBÍREJ MATERIÁL A PŘEŽIJ"
 exploration_title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
 exploration_title.add_theme_font_size_override("font_size", 23)
 explore_hud.add_child(exploration_title)
 var back := Button.new()
 back.text = "← ZPĚT DO BUNKRU"
 back.pressed.connect(_exit_exploration)
 explore_hud.add_child(back)
 var filler := Control.new()
 filler.size_flags_vertical = Control.SIZE_EXPAND_FILL
 explore_hud.add_child(filler)
 var movement := HBoxContainer.new()
 movement.alignment = BoxContainer.ALIGNMENT_CENTER
 explore_hud.add_child(movement)
 _hold_button(movement, "◀", Vector2(-1, 0))
 _hold_button(movement, "▲", Vector2(0, -1))
 _hold_button(movement, "▼", Vector2(0, 1))
 _hold_button(movement, "▶", Vector2(1, 0))
 _action(movement, "⚔ ÚTOK", _attack)
 _action(movement, "⚙ SBĚR", _scavenge)

func _action(parent: Node, caption: String, callback: Callable) -> void:
 var button := Button.new()
 button.text = caption
 button.custom_minimum_size = Vector2(125, 52)
 button.pressed.connect(callback)
 parent.add_child(button)

func _hold_button(parent: Node, caption: String, direction: Vector2) -> void:
 var button := Button.new()
 button.text = caption
 button.custom_minimum_size = Vector2(72, 65)
 button.button_down.connect(func(): move_direction = direction)
 button.button_up.connect(func(): move_direction = Vector2.ZERO)
 parent.add_child(button)

func _make_world(viewport: SubViewport) -> void:
 var world := Node3D.new()
 viewport.add_child(world)
 var environment := WorldEnvironment.new()
 var env := Environment.new()
 env.background_mode = Environment.BG_COLOR
 env.background_color = Color("#131722")
 env.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
 env.ambient_light_color = Color("#66738a")
 env.ambient_light_energy = 0.5
 environment.environment = env
 world.add_child(environment)
 var sun := DirectionalLight3D.new()
 sun.rotation_degrees = Vector3(-55, -25, 0)
 sun.light_energy = 0.8
 world.add_child(sun)
 _cube(world, Vector3(0, -0.35, 0), Vector3(70, 0.5, 70), Color("#353a3b"))
 for i in range(18):
  var x := float((i * 17) % 43 - 21)
  var z := float((i * 23) % 39 - 19)
  _cube(world, Vector3(x, 1.2, z), Vector3(2.5 + i % 3, 2.4, 2.0), Color("#4a4b4b"))
 for i in range(6):
  var enemy := Node3D.new()
  enemy.position = Vector3(-12 + i * 5, 0, -11 + (i % 2) * 5)
  world.add_child(enemy)
  _cube(enemy, Vector3(0, 0.85, 0), Vector3(0.7, 1.7, 0.6), Color("#67926a"))
  _cube(enemy, Vector3(0, 1.9, 0), Vector3(0.55, 0.55, 0.55), Color("#8ab08c"))
  enemy_nodes.append(enemy)
 world_actor = Node3D.new()
 world.add_child(world_actor)
 _cube(world_actor, Vector3(0, 0.9, 0), Vector3(0.7, 1.8, 0.6), Color("#d4a260"))
 _cube(world_actor, Vector3(0, 2.0, 0), Vector3(0.5, 0.5, 0.5), Color("#e6c9a0"))
 world_camera = Camera3D.new()
 world_camera.position = Vector3(0, 14, 18)
 world_camera.rotation_degrees = Vector3(-34, 0, 0)
 world_camera.current = true
 world.add_child(world_camera)

func _cube(parent: Node3D, pos: Vector3, dimensions: Vector3, color: Color) -> void:
 var body := MeshInstance3D.new()
 var mesh := BoxMesh.new()
 mesh.size = dimensions
 body.mesh = mesh
 var mat := StandardMaterial3D.new()
 mat.albedo_color = color
 mat.roughness = 0.9
 body.material_override = mat
 body.position = pos
 parent.add_child(body)

func _process(delta: float) -> void:
 if not in_exploration:
  return
 var keyboard := Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
 var direction := move_direction if move_direction.length() > 0.01 else keyboard
 if direction.length() > 0.01:
  world_actor.position.x = clampf(world_actor.position.x + direction.x * delta * 6.0, -30.0, 30.0)
  world_actor.position.z = clampf(world_actor.position.z + direction.y * delta * 6.0, -30.0, 30.0)
  world_actor.rotation.y = atan2(direction.x, direction.y)
 world_camera.position = world_actor.position + Vector3(0, 14, 18)
 for enemy in enemy_nodes:
  if not is_instance_valid(enemy):
   continue
  var distance := enemy.position.distance_to(world_actor.position)
  if distance < 9.0 and distance > 1.1:
   enemy.position = enemy.position.move_toward(world_actor.position, delta * 0.85)
  if distance < 1.2:
   player_health = maxi(0, player_health - int(ceil(delta * 6)))
 if player_health <= 0:
  _exit_exploration()
  morale = maxi(0, morale - 15)
  log_message = "Výprava selhala. Přeživší se vrátil zraněný."
  player_health = 100
  _refresh()

func _attack() -> void:
 for enemy in enemy_nodes:
  if is_instance_valid(enemy) and enemy.position.distance_to(world_actor.position) < 3.5:
   enemy.queue_free()
   enemies_defeated += 1
   scrap += 6
   log_message = "Hrozba odstraněna. +6 materiálu."
   return
 log_message = "Žádný nepřítel v dosahu."

func _scavenge() -> void:
 if randf() < 0.65:
  scrap += 12
  food += 3
  log_message = "Nalezené zásoby: +12 materiálu, +3 jídlo."
 else:
  player_health = maxi(1, player_health - 15)
  log_message = "Past! -15 zdraví."
 event_label.text = log_message

func _enter_exploration() -> void:
 in_exploration = true
 surface.visible = true
 explore_hud.visible = true
 overlay.visible = false

func _exit_exploration() -> void:
 in_exploration = false
 move_direction = Vector2.ZERO
 surface.visible = false
 explore_hud.visible = false
 overlay.visible = true
 _refresh()

func _build(kind: String) -> void:
 var cost: int = ROOM_DATA[kind]["cost"]
 if scrap < cost:
  log_message = "Nedostatek materiálu pro: " + kind
 elif rooms.size() >= 24:
  log_message = "Bunkr je plný. Kapacita 24 místností."
 else:
  scrap -= cost
  rooms.append(kind)
  defense += 14 if kind == "Obrana" else 0
  morale += 3 if kind == "Ložnice" else 0
  log_message = "Dokončena stavba: " + kind
 _refresh()

func _next_day() -> void:
 day += 1
 food += rooms.count("Farma") * 11 - people * 2
 power += rooms.count("Generátor") * 16 - rooms.size() * 4
 scrap += rooms.count("Dílna") * 7
 if rooms.count("Ošetřovna") > 0:
  morale += 2
 if food <= 0 or power <= 0:
  morale -= 10
  if day % 2 == 0:
   people = maxi(1, people - 1)
  log_message = "KRIZE! Nedostatek jídla nebo energie."
 elif day % 3 == 0:
  var raid := randi_range(12, 55) + day
  if defense >= raid:
   scrap += 20
   log_message = "Útok odražen! Získáno 20 materiálu."
  else:
   scrap = maxi(0, scrap - 20)
   morale -= 12
   log_message = "ZOMBIE PRORAZILY OBRANU! Ztráta zásob."
 else:
  log_message = "Další den přežit. Připrav základnu na další útok."
 food = maxi(0, food)
 power = maxi(0, power)
 morale = clampi(morale, 0, 100)
 _refresh()
 _save()

func _refresh() -> void:
 header.text = "DEN %d  |  LIDÉ %d  |  JÍDLO %d  |  ENERGIE %d  |  MATERIÁL %d  |  OBRANA %d  |  MORALE %d%%" % [day, people, food, power, scrap, defense, morale]
 event_label.text = log_message
 for child in bunker_grid.get_children():
  bunker_grid.remove_child(child)
  child.queue_free()
 for i in range(maxi(9, rooms.size())):
  var panel := PanelContainer.new()
  panel.custom_minimum_size = Vector2(170, 90)
  panel.size_flags_horizontal = Control.SIZE_EXPAND_FILL
  bunker_grid.add_child(panel)
  var label := Label.new()
  label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
  label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
  label.add_theme_font_size_override("font_size", 18)
  if i < rooms.size():
   var kind := rooms[i]
   label.text = str(ROOM_DATA[kind]["symbol"]) + "\n" + kind
   label.add_theme_color_override("font_color", Color(str(ROOM_DATA[kind]["color"])))
  else:
   label.text = "░  NEVYBUDOVÁNO"
   label.add_theme_color_override("font_color", Color("#59606a"))
  panel.add_child(label)
 for button in build_buttons:
  var kind := button.text.split("  ")[0]
  button.disabled = scrap < int(ROOM_DATA[kind]["cost"]) or rooms.size() >= 24

func _save() -> void:
 var file := FileAccess.open(SAVE_PATH, FileAccess.WRITE)
 if file == null:
  log_message = "Chyba při ukládání."
  return
 file.store_string(JSON.stringify({"version": 2, "day": day, "food": food, "power": power, "scrap": scrap, "people": people, "defense": defense, "morale": morale, "rooms": rooms}))
 file.close()
 log_message = "Postup uložen."
 event_label.text = log_message

func _load() -> void:
 if not FileAccess.file_exists(SAVE_PATH):
  log_message = "Uložená hra neexistuje."
  _refresh()
  return
 var data = JSON.parse_string(FileAccess.get_file_as_string(SAVE_PATH))
 if typeof(data) != TYPE_DICTIONARY:
  log_message = "Uložená hra je poškozená."
  _refresh()
  return
 day = maxi(1, int(data.get("day", 1)))
 food = maxi(0, int(data.get("food", 65)))
 power = maxi(0, int(data.get("power", 80)))
 scrap = maxi(0, int(data.get("scrap", 125)))
 people = maxi(1, int(data.get("people", 8)))
 defense = maxi(0, int(data.get("defense", 30)))
 morale = clampi(int(data.get("morale", 75)), 0, 100)
 rooms.clear()
 for room in data.get("rooms", []):
  if ROOM_DATA.has(str(room)):
   rooms.append(str(room))
 if rooms.is_empty():
  rooms.append("Velitelství")
 log_message = "Postup obnoven."
 _refresh()
