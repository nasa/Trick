exec(open("./Modified_data/realtime.py").read())

# Variable Server Data should be copied at top of frame.
trick.var_set_copy_mode(2)

Manip2D.robot.kinemat.joint_q[0] = trick.attach_units("degrees",45.0)
Manip2D.robot.kinemat.joint_q[1] = trick.attach_units("degrees",45.0)

Manip2D.robot.kinemat.joint_w[0] = 0.0
Manip2D.robot.kinemat.joint_w[1] = 0.0

Manip2D.robot.controller.Kp = 2.0
Manip2D.robot.controller.Kd = 0.3
Manip2D.robot.kinemat.checkSingularities = False

armIntegLoop.getIntegrator(trick.Euler, Manip2D.robot.ndof)

graphics = False

trick.frame_log_on()

if graphics:
    #==========================================
    # Start the Graphics Client
    #==========================================
    varServerPort = trick.var_server_get_port();
    RobotDisplay_path = "models/graphics/build/RobotDisplay.jar"
    
    if (os.path.isfile(RobotDisplay_path)) :
        RobotDisplay_cmd = "java -jar " \
                       + RobotDisplay_path \
                       + " " + str(varServerPort) + " &" ;
        print(RobotDisplay_cmd)
        os.system( RobotDisplay_cmd);
    else :
        print('==================================================================================')
        print('RobotDisplay needs to be built. Please \"cd\" into ../models/graphics and type \"make\".')
        print('==================================================================================')
