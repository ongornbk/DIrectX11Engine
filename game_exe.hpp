Engine::Update();
lua::execute(gamescene_update);
Timer::Update();
RendererManager::Update();
UserInterface::Update();
TileMap::Update();
RenderLayer::CleanUp();
RenderLayer::Update();
RenderLayer::Sort();
RenderLayer::QSort();
Input::Update();
Engine::Render();
GarbageCollector::Update();
RendererManager::Render();
TileMap::Render();
RenderLayer::Render();
UserInterface::Render();
lua::execute(RENDERAFTER);
