extends Control

var food := 60
var power := 80
var scrap := 100
var people := 8
var day := 1
var rooms: Array[String] = ["Velitelství", "Generátor", "Ložnice"]
var info: Label
var room_list: Label

func _ready() -> void:
	var bg := ColorRect.new()
	bg.color = Color("#101820")
	bg.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	add_child(bg)
	var layout := VBoxContainer.new()
	layout.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	layout.add_theme_constant_override("separation", 15)
	add_child(layout)
	var title := Label.new()
	title.text = "LAST HAVEN: AFTER DARK | ALPHA 0.1"
	title.add_theme_font_size_override("font_size", 32)
	layout.add_child(title)
	info = Label.new()
	info.add_theme_font_size_override("font_size", 22)
	layout.add_child(info)
	room_list = Label.new()
	room_list.add_theme_font_size_override("font_size", 22)
	layout.add_child(room_list)
	var actions := HBoxContainer.new()
	layout.add_child(actions)
	_add_button(actions, "Generátor (40)", "Generátor", 40)
	_add_button(actions, "Farma (30)", "Farma", 30)
	_add_button(actions, "Ložnice (25)", "Ložnice", 25)
	_add_button(actions, "Ošetřovna (35)", "Ošetřovna", 35)
	var next := Button.new()
	next.text = "DALŠÍ DEN"
	next.pressed.connect(_next_day)
	layout.add_child(next)
	var save := Button.new()
	save.text = "ULOŽIT"
	save.pressed.connect(_save)
	layout.add_child(save)
	var load_button := Button.new()
	load_button.text = "NAČÍST"
	load_button.pressed.connect(_load)
	layout.add_child(load_button)
	_refresh()

func _add_button(parent: HBoxContainer, caption: String, kind: String, cost: int) -> void:
	var button := Button.new()
	button.text = caption
	button.pressed.connect(func(): _build(kind, cost))
	parent.add_child(button)

func _build(kind: String, cost: int) -> void:
	if scrap < cost:
		return
	scrap -= cost
	rooms.append(kind)
	_refresh()

func _next_day() -> void:
	day += 1
	food += rooms.count("Farma") * 8 - people * 2
	power += rooms.count("Generátor") * 12 - rooms.size() * 3
	if day % 3 == 0:
		scrap = maxi(0, scrap - 8)
	food = maxi(0, food)
	power = maxi(0, power)
	_refresh()

func _refresh() -> void:
	info.text = "Den %d | Lidé %d | Jídlo %d | Energie %d | Materiál %d" % [day, people, food, power, scrap]
	room_list.text = "Místnosti: " + ", ".join(rooms)

func _save() -> void:
	var file := FileAccess.open("user://last_haven_save.json", FileAccess.WRITE)
	if file:
		file.store_string(JSON.stringify({"day": day, "food": food, "power": power, "scrap": scrap, "people": people, "rooms": rooms}))

func _load() -> void:
	if not FileAccess.file_exists("user://last_haven_save.json"):
		return
	var data = JSON.parse_string(FileAccess.get_file_as_string("user://last_haven_save.json"))
	if typeof(data) != TYPE_DICTIONARY:
		return
	day = int(data.get("day", 1))
	food = int(data.get("food", 60))
	power = int(data.get("power", 80))
	scrap = int(data.get("scrap", 100))
	people = int(data.get("people", 8))
	rooms.clear()
	for room in data.get("rooms", []):
		rooms.append(str(room))
	_refresh()
