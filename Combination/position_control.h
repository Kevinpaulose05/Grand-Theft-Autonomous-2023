
void position_control() {
    x_target=3300;
    y_target=3900;
    moveToTargetCoordinates(x_target, y_target, x_center, y_center);   

    // Add a delay to control loop execution
    handleUDPServer();
    delay(40);  // Adjust the delay as needed
}
