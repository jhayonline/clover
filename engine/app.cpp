#include "../include/app.hpp"

Router App::routes() {
  Router router;

  // Mount all controller routes
  // router.mount(HelloController::routes());

  return router;
}
