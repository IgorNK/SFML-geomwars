# Geometry wars in SFML
A small educational game project aimed at getting familiar with entity component systems and SFML.
## Build instructions
### Linux
Create a separate build directory outside of the source folder, i.e. "SFML-geomwars_build", and navigate to it.
This example uses Ninja compiler:
```
> cmake -G Ninja ../SFML-geomwars
> cmake --build .
> git submodule update --init
> cd ./src
> ./sfmlgame
```
### Windows
Should be the same as Linux.

## Configuration
When you hit "save" in the debug gui window, the game creates a configuration file that mimics what's in the ```src/resources/config.txt```, but overwrites it with values changed during run-time.
When the ```user_config.txt``` file is present in the executable folder, the game prioritizes this configuration file over the default one.

## ECS
### Entities
Entities are managed by the EntityManager object stored as a private variable in Game class (m_entity_manager).
Entity storage is implemented twice: as a std::vector of all entities, and as a std::map of entities by tag (Tag::Enemies, Tag::Bullets, etc.)
To create an entity, you call ```m_entity_manager.add_entity(Tag tag);```, and this call returns a new shared pointer for a newly created entity.
### Components
After you have a shared pointer to newly created entity, you populate it with components.
Entity class houses a limited number of member variables for every possible component type. For example, transform:
```entity->transform = std::make_shared<CTransform>(CTransform(...args));```
For ease of use in debugging GUI, component.h houses a couple std::maps coupling component type enums with string values (component_names, name_components).
Components are supposed to house data only. All game logic is operated by systems.
### Systems
Systems query all entities (by tag, if necessary) and do things on components housed within.
For example:
```
for (const std::shared_ptr<Entity> entity : m_entity_manager.get_entities()) {
  // ... do some logic with entity
}
```
Or you can query entities by tag:
```
for (const std::shared_ptr<Entity> bullet : m_entity_manager.get_entities(Tag::Bullets)) {
  // ... do bullet stuff
}
```
#### sRender
This system queries all entities that have renderable components (CShape, CText, CLine) and transform (CTransform) to display them in the window by calling draw function on SFML RenderWindow: ``` m_window.draw(shape);```
A call to ```ImGui::SFML::Render(m_window);``` displays debug interface stuff.
```m_window.clear();``` clears the buffer;
```m_window.display();``` swaps back and front buffer.

#### sTimers
This system queries all entities that have components that have counters that need decrementing: CInvincibility, CLifespan, CHealth, cWeapon and cSpecialWeapon and cEmitter.
Also, it decrements a counter for closing the game when Escape button is pressed.

#### sGui
This is debug interface written using ImGui-SFML.

#### sEffects
System currently houses a single effect: flickering player shape when he's invincible.

#### sMovement
This system queries all entities that have CTransform and CVelocity and updates Transform based on said Velocity.
Also, it handles movement limits for the player (so that he doesn't escape the play area bounds) and bouncing of enemies from the edges of the screen.

#### sEmitters
This system is used exclusively to handle spawning new entities from CEmitter components.

#### sDamageReact
Visual effect for expanding the shapes when they are hit. Done by querying health components and checking for timer progression: ```health.react_countdown```.

#### sEnemySpawner and sPlayerSpawner
As described, these systems handle spawning new enemies according to timer set in Game class member variable, and spawning a player when he is not present.

#### sUserInput and sInputHandilng
sUserInput gets input values from keyboard presses and SFML window events, and passes them into CInput component, or changes corresponding Game member boolean variables, such as m_paused, m_running and various others, defined for enabling and disabling systems: m_sGUI, m_sRender etc.
sInputHandling queries cInput component and updates that entity's velocity, or fires off shoot() nad shootSpecialWeapon() methods.

#### sLifespan
System for making objects with lifespan gradually dissappear into non-existence.


I hope the game itself is fun, enjoy!
